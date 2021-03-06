
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
            "enter": function() {
                send(" == District 21 ==\n\n");
            },
            "prompt": function() {
                send("Unidentified Clone, enter your ID: ");
            },
            "processInput": function(input) {
                if (input.substr(0, 1) === "c" || input.substr(0, 1) === "C") {
                    input = input.substr(1);
                }

                var cloneId = parseInt(input, 10);
                if (input.length < 2 || input.length > 5 || isNaN(cloneId) || cloneId < 14) {
                    send("Not a valid Clone ID number. A valid Clone ID a number ranging from 14 " +
                         "through 99999\n");
                    return;
                }

                var player;
                if (cloneId >= 14) {
                    player = Realm.getPlayer("C" + cloneId);
                }

                if (player) {
                    this.setPlayer(player);
                    this.setState("AskingPassword");
                } else {
                    signUpData.userName = "C" + cloneId;
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
                send(this.player.name + ", enter your passphrase: ");
            },
            "processInput": function(input) {
                if (this.player.matchesPassword(input)) {
                    LogUtil.logSessionEvent(this._session.source, "Authentication success for " +
                                            "player " + this.player.name);

                    if (this.player.isOnline()) {
                        send("Rejected. Clone is already active from another terminal.\n");
                        this.setState("SessionClosed");
                        return;
                    }

                    send(("%1 Authenticated. Type *help* for instructions.\n" +
                          "\n" +
                          "You open your eyes.\n")
                         .arg(this.player.name));
                    this.setState("SignedIn");
                }  else {
                    LogUtil.logSessionEvent(this._session.source, "Authentication failed for " +
                                            "player " + this.player.name);

                    send("Authentication Failure.\n\n", Color.Red);
                }
            }
        },

        "AskingUserNameConfirmation": {
            "prompt": function() {
                send("\n%1 Not Registered. Register? ".arg(signUpData.userName));
            },
            "processInput": function(input) {
                var answer = input.toLower();
                if (answer === "yes" || answer === "y") {
                    this.setState("AskingSignUpPassword");
                } else if (answer === "no" || answer === "n") {
                    this.setState("SignInAborted");
                } else {
                    send("Enter *yes* or *no*.\n");
                }
            }
        },

        "AskingSignUpPassword": {
            "enter": function() {
                send({ "inputType": "password" });
            },
            "prompt": function() {
                send("\nChoose a passphrase: ");
            },
            "processInput": function(input) {
                if (input.length < 6) {
                    send("Passphrase should be at least 6 characters.\n", Color.Red);
                    return;
                }
                if (input.toLower() === signUpData.userName.toLower()) {
                    send("Passphrase may not equal Clone ID.\n", Color.Red);
                    return;
                }
                if (input === "123456" || input === "654321") {
                    send("Passphrase too simple.\n", Color.Red);
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
                send("Confirm passphrase: ");
            },
            "processInput": function(input) {
                if (signUpData.password === input) {
                    send("Passphrase confirmed.\n", Color.Green);
                    this.setState("AskingGender");
                } else {
                    send("Passphrases don't match.\n", Color.Red);
                    this.setState("AskingSignUpPassword");
                }
            }
        },

        "AskingGender": {
            "enter": function() {
                send("\n" +
                     "Choose a gender for your clone:");
            },
            "processInput": function(input) {
                var answer = input.toLower();
                if (answer === "male" || answer === "m") {
                    send("Male clone selected.\n", Color.Green);
                    signUpData.gender = "male";
                    this.setState("AskingSignUpConfirmation");
                } else if (answer === "female" || answer === "f") {
                    send("Female clone selected.\n", Color.Green);
                    signUpData.gender = "female";
                    this.setState("AskingSignUpConfirmation");
                } else {
                    send("\n" +
                         "Choose *male* or *female*.\n");
                }
            }
        },

        "AskingSignUpConfirmation": {
            "enter": function() {
                send("\n" +
                     "Clone is ready for production.\n");
            },
            "prompt": function() {
                send("\n" +
                     "Continue?\n");
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
                        send("Clone ID already taken. Please re-register.\n");
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

                    send(("%1 Created. Type *help* for instructions.\n").arg(this.player.name) +
                         "\n" +
                         "You open your eyes.\n");

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
