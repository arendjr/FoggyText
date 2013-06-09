
/**
 * Constructor.
 *
 * @note Don't instantiate this class directly, use Realm.createObject("Player") instead.
 */
function Player() {
}

/**
 * This method is called whenever the player has entered a room.
 */
Player.prototype.enteredRoom = function() {

    this.send(this.currentRoom.lookAtBy(this));
};
