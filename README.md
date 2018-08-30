## Current master branch on Unreal 4.19.
### The implementation is pretty basic and should be useable on other versions (assets might not be compatible with older versions)

# NotificationBackbone
  Send notifications across your whole project from anywhere to anywhere.
  Notifications are handled per feed.
  
  ### Feeds 
  
    * are defined by name (case insensitive) (create feeds dynamically)
    * get created and destroyed as needed
    * can be blocked to pervent dispatching (usefull for delayed dispatching and loading times)
    * can have multiple subscribers
    * have their own settings (see project settings Plugins->NotificationBackboneSettings)
      * delay dispatching
      * cache notifications
      * ...
 
  The plugin comes with demo widgets that help test/debug and give you an hint on how to use it.

### Useage ideas
  * Simple notifications for quest state reached, item pickup...
  * Create a feed for dmg done to the player to pop up dmg numbers
  * Send JSON structures across your project
  * ...
