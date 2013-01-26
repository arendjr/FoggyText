
var SessionState = {
    "SessionClosed": 0,
    "SigningIn": 1,
    "SignedIn": 2
};

/**
 * This class gets instantiated for every user session. It's responsible for
 * authenticating users, signing up new users, and creating their characters.
 *
 * Right after instantiation, setSession() is called to set a reference to the
 * native Session object. You can use the setSessionState() method to
 * communicate the session state. Set the state to SessionState.SessionClosed
 * when you want the session to be closed, and to SessionState.SignedIn when a
 * user has been authenticated. Before entering the signed-in state you should
 * set the player object using the setPlayer() method.
 */
function SessionHandler() {

    var self = this;
    function send(message, color) {
        if (typeof message !== "string") {
            message = JSON.stringify(message);
        }
        message = Util.processHighlights(message);
        if (color !== undefined) {
            message = Util.colorize(message, color);
        }
        self.send(message);
    }

    var signUpData = {};

    this.states = {
        "AskingUserName": {
            "prompt": function() {
                send("What is your name? ");
            },
            "processInput": function(input) {
                var userName = Util.validateUserName(input.toLower());
                if (userName.length < 3) {
                    if (!userName.isEmpty()) {
                        send("I'm sorry, but your name should consist of at least 3 letters.\n");
                    }
                    return;
                }

                var player = Realm.getPlayer(userName);
                if (player) {
                    this.setPlayer(player);
                    this.setState("AskingPassword");
                } else if (Realm.reservedNames().contains(userName)) {
                    send("Yeah right, like I believe that...\n");
                } else {
                    signUpData.userName = userName;
                    this.setState("AskingUserNameConfirmation");
                }
            }
        },

        "AskingPassword": {
            "enter": function() {
                send({ "inputType": "password" });
            },
            "exit": function() {
                send({ "inputType": "text" });
            },
            "prompt": function() {
                send("Please enter your password: ");
            },
            "processInput": function(input) {
                if (this.player.matchesPassword(input)) {
                    LogUtil.logSessionEvent(this._session.source, "Authentication success for " +
                                            "player " + this.player.name);

                    if (this.player.isOnline()) {
                        send("Cannot sign you in because you're already signed in from another " +
                             "location.\n");
                        this.setState("SessionClosed");
                        return;
                    }

                    send("Welcome back, %1. Type *help* if you're feeling lost.\n"
                         .arg(this.player.name));
                    this.setState("SignedIn");
                }  else {
                    LogUtil.logSessionEvent(this._session.source, "Authentication failed for " +
                                            "player " + this.player.name);

                    send("Password incorrect.\n");
                }
            }
        },

        "AskingUserNameConfirmation": {
            "prompt": function() {
                send("%1, did I get that right? ".arg(signUpData.userName));
            },
            "processInput": function(input) {
                var answer = input.toLower();
                if (answer === "yes" || answer === "y") {
                    this.setState("AskingSignUpPassword");
                } else if (answer === "no" || answer === "n") {
                    this.setState("SignInAborted");
                } else {
                    send("Please answer with yes or no.\n");
                }
            }
        },

        "AskingSignUpPassword": {
            "enter": function() {
                send({ "inputType": "password" });
            },
            "prompt": function() {
                send("Please choose a password: ");
            },
            "processInput": function(input) {
                if (input.length < 6) {
                    send("Please choose a password of at least 6 characters.\n", Color.Red);
                    return;
                }
                if (input.toLower() === signUpData.userName.toLower()) {
                    send("Your password and your username may not be the same.\n", Color.Red);
                    return;
                }
                if (input === "123456" || input === "654321") {
                    send("Sorry, that password is too simple.\n", Color.Red);
                    return;
                }
                signUpData.password = input;
                this.setState("AskingSignUpPasswordConfirmation");
            }
        },

        "AskingSignUpPasswordConfirmation": {
            "exit": function() {
                send({ "inputType": "text" });
            },
            "prompt": function() {
                send("Please confirm your password: ");
            },
            "processInput": function(input) {
                if (signUpData.password === input) {
                    send("Password confirmed.\n", Color.Green);
                    this.setState("AskingGender");
                } else {
                    send("Passwords don't match.\n", Color.Red);
                    this.setState("AskingSignUpPassword");
                }
            }
        },

        "AskingGender": {
            "enter": function() {
                send("\n" +
                     "Please select which gender you would like your character to be.\n" +
                     "Your gender has a minor influence on the physique of your character.");
            },
            "prompt": function() {
                send("\n" +
                     "Please choose *male* or *female*.\n");
            },
            "processInput": function(input) {
                var answer = input.toLower();
                if (answer === "male" || answer === "m") {
                    send("\nYou have chosen to be male.\n", Color.Green);
                    signUpData.gender = "male";
                    this.setState("AskingSignUpConfirmation");
                } else if (answer === "female" || answer === "f") {
                    send("\nYou have chosen to be female.\n", Color.Green);
                    signUpData.gender = "female";
                    this.setState("AskingSignUpConfirmation");
                }
            }
        },

        "AskingSignUpConfirmation": {
            "enter": function() {
                send("\n" +
                     "You will become a %1 soldier.\n".arg(signUpData.gender) +
                     "\n");
            },
            "prompt": function() {
                send("\n" +
                     "Are you ready to create this character?\n");
            },
            "processInput": function(input) {
                var answer = input.toLower();
                if (answer === "yes" || answer === "y") {
                    var humanRace = Realm.getObject("Race", 2);
                    var soldierClass = Realm.getObject("Class", 3);

                    var stats = humanRace.stats.plus(soldierClass.stats);

                    var height = humanRace.height;
                    var weight = humanRace.weight;
                    if (signUpData.gender === "male") {
                        stats[STRENGTH]++;
                        height += 10;
                        weight += 15;
                    } else {
                        stats[DEXTERITY]++;
                        weight -= 5;
                    }

                    for (var i = 0; i < 9; i++) {
                        var attr = randomInt(0, 6);
                        stats[attr]++;
                        if (attr === STRENGTH) {
                            weight++;
                        } else if (attr === DEXTERITY) {
                            if (byChance(1, 2)) {
                                height--;
                            }
                        } else if (attr === INTELLIGENCE) {
                            height++;
                        }
                    }

                    var player = Realm.getPlayer(signUpData.userName);
                    if (player) {
                        send("Uh-oh, it appears someone has claimed your character name while " +
                             "you were creating yours. I'm terribly sorry, but it appears you " +
                             "will have to start over.\n");
                        this.setState("SessionClosed");
                        return;
                    }

                    player = Realm.createObject("Player");
                    player.admin = Realm.players().isEmpty();
                    player.name = signUpData.userName;
                    player.race = humanRace;
                    player.characterClass = soldierClass;
                    player.gender = signUpData.gender;
                    player.stats = stats;
                    player.height = height;
                    player.weight = weight;
                    player.currentRoom = humanRace.startingRoom;

                    player.hp = player.maxHp;
                    player.mp = player.maxMp;

                    player.setPassword(signUpData.password);

                    signUpData = {};

                    LogUtil.logSessionEvent(this._session.source, "Character created for player " +
                                            player.name);

                    send("\nWelcome to %1, %2.\n".arg(Realm.name, player.name));

                    this.setPlayer(player);
                    this.setState("SignedIn");
                } else if (answer === "no" || answer === "n" ||
                           answer === "back" || answer === "b") {
                    this.setState("AskingGender");
                } else {
                    send("Please answer with yes or no.\n");
                }
            }
        },

        "SignedIn": {
            "enter": function() {
                this.setSessionState(SessionState.SignedIn);
            }
        },

        "SignInAborted": {
            "enter": function() {
                send("Okay, bye.");
                this.setState("SessionClosed");
            }
        },

        "SessionClosed": {
            "enter": function() {
                this.setSessionState(SessionState.SessionClosed);
            }
        }
    };

    this.state = null;

    this.player = null;

    this._session = null;
}

SessionHandler.prototype.setSession = function(session) {

    this._session = session;

    this.setState("AskingUserName");
    if (this.state.prompt) {
        this.state.prompt.call(this);
    }
};

SessionHandler.prototype.setState = function(name) {

    if (this.states[name] === this.state) {
        return;
    }

    if (this.state && this.state.exit) {
        this.state.exit.call(this);
    }

    this.state = this.states[name];

    if (this.state.enter) {
        this.state.enter.call(this);
    }
};

SessionHandler.prototype.setPlayer = function(player) {

    this.player = player;

    this._session.setPlayer(player);
};

SessionHandler.prototype.setSessionState = function(sessionState) {

    this._session.setSessionState(sessionState);
};

SessionHandler.prototype.send = function(message) {

    this._session.send(message);
};

SessionHandler.prototype.processSignIn = function(input) {

    this.state.processInput.call(this, input);

    if (this.state.prompt) {
        this.state.prompt.call(this);
    }
};
