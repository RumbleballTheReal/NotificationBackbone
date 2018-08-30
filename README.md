## Current master branch on Unreal 4.19.
### The implementation is pretty basic and should be useable on other versions (assets might not be compatible with older versions)

# NotificationBackbone
  Send notifications across your whole project from anywhere to anywhere.
  Notifications are handled per feed.
  Feeds 
    -are defined by name (case insensitive)
    -get created and destroyed as needed
    -feed can have multiple subscribers
    -has its own settings (see project settings Plugins->NotificationBackboneSettings)
      -delay dispatching
      -cache notifications
      - ...
 
  The plugin comes with demo widgets that help test/debug and give you an hint on how to use it.
