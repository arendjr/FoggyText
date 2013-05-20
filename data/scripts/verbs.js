
function Verb(tenses) {

    if (typeof tenses === "string") {
        tenses = { "singular": tenses };
    }

    function singularToThirdPerson(singular) {
        var lastCharacter = singular.substr(-1);
        if (Util.isVowel(lastCharacter)) {
            return singular + "es";
        } else if (lastCharacter === "s" || lastCharacter === "x") {
            return singular + "es";
        } else {
            return singular + "s";
        }
    }

    function singularToContinuous(singular) {
        var lastCharacter = singular.substr(-1), secondLastCharacter = singular.substr(-2, 1);
        if (lastCharacter === "e") {
            if (secondLastCharacter === "i") {
                return singular.left(singular.length - 2) + "ying";
            } else {
                return singular.left(singular.length - 1) + "ing";
            }
        } else {
            if (Util.isVowel(secondLastCharacter) && !Util.isVowel(lastCharacter)) {
                return singular + lastCharacter + "ing";
            } else {
                return singular + "ing";
            }
        }
    }

    this.singular = tenses.singular;
    this.plural = tenses.plural || this.singular;
    this.secondPerson = tenses.secondPerson || this.plural;
    this.thirdPerson = tenses.thirdPerson || singularToThirdPerson(this.singular);
    this.continuous = tenses.continuous || singularToContinuous(this.singular);
}

var VERBS = {
    "attack": new Verb("attack"),
    "be": new Verb({
        "singular": "am",
        "thirdPerson": "is",
        "plural": "are",
        "continuous": "being"
    }),
    "can": new Verb({
        "singular": "can",
        "thirdPerson": "can",
        "continuous": "able to"
    }),
    "do": new Verb("do"),
    "fight": new Verb("fight"),
    "give": new Verb("give"),
    "go": new Verb("go"),
    "guard": new Verb("guard"),
    "hang": new Verb("hang"),
    "have": new Verb({
        "singular": "have",
        "thirdPerson": "has",
    }),
    "lie": new Verb("lie"),
    "run": new Verb("run"),
    "shout": new Verb("shout"),
    "stand": new Verb("stand"),
    "take": new Verb("take"),
    "talk": new Verb("talk"),
    "walk": new Verb("walk")
};
