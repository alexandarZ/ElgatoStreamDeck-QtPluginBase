# ElgatoStreamDeck-QtPluginBase
Plugin base for Elgato Stream Deck application made in Qt Creator  
Made with QtCreator 4.9.0  
Tested with 15 keys ElgatoStream deck  

## About classes  

*ESDPlugin* - Plugin base, it communicates with stream deck application over QWebSocket, by sending and receiving JSON messages  

*ESDDeviceModel* - JSON model for getting info about device which is connected on the PC  

*ESDActionModel* - JSON model for device actions

*MyPlugin* - Example of plugin who use ESDPlugin for receiving KeyUp and KeyDown messages from StreamDeck application

## Debugging

1) Quit the Stream Deck application
2) Build project in debug configuration. It will create log file where you can see all debug messages
3) Copy build directory with Qt debug dlls into Elgato Stream Deck **plugins** folder.

    **macOS** - You will find it here:
    ~/Library/Application Support/com.elgato.StreamDeck/Plugins/

    **Windows** - You will find it here:
    %appdata%\Roaming\Elgato\StreamDeck\Plugins\com.elgato.StreamDeck\Plugins\

4) Rename directory to contain unique URI, for example com.example.myplugin.sdPlugin
5) Edit the manifest.json file to change the actions UUID, name, plugin name, author ...
6) If you now launch the Stream Deck application, your plugin should appear in the Custom category in the Actions list.

## Support

- If you see any **bug** - Please raise new issue. I'll do my best to fix it as soon as possible!
- Want to work with me on this code? Just fork stuff and open PR with changes
