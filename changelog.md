# EndscreenTweaks Changelog
## v1.5.3
- True story: for some reason, some of my built-in replacements weren't being parsed properly, so I recompiled it again to find out what the issue was. Seems like I was getting all worked up for nothing, but I'm leaving the log calls in there anyway in case something goes wrong.
- Add [Saritahh](https://github.com/DeepRsnanceX) to mod developers list as originally promised.
## v1.5.2
- Some cleanup from behind the scenes.
- Improved font accuracy for `Oxygene One`, thank you [Saritahh](https://github.com/DeepRsnanceX)!
## v1.5.1
- Fix a stupid C++ crash for shuffling empty data types.
## v1.5.0
- Refactor source code into multiple files.
- Add "Custom "Level Complete" Text" options. (suggested by [Saritahh](https://github.com/DeepRsnanceX))
  - Replacement options include "Oxygene One", "Images", and "Combined".
  - Additional options include replacing the "Level Complete" text when completing a level before the end screen, or generating a separate replacement. (Also Replace "Level Complete" in PlayLayer)
  - Additional option to re-scale "Images" smaller than a certain dimension. (Custom "Level Complete" Images Scaling Correction)
## v1.4.2
- Port to 2.2074.
## v1.4.1
- Added "Custom Quotes Only" option, by popular demand. (If no custom quotes are found, the mod will fall back to anything enabled from the default set of quotes.)
- Skip to v1.4.1 for consistency with DeathScreenTweaks.
## v1.3.0
- Add config directory shortcut button.
- Add "Fluked From %" options for Classic levels. (suggested by [yolodomo](https://discord.com/users/708984489283682327))
## v1.2.1
- Fix a crash from selecting Fonts 1 through 9 (even though they're all objectively ugly). Thanks again, hiimjustin000!
## v1.2.0
- Rebranded to `EndscreenTweaks`, and supersede past copies of Relative's Custom Endscreen Text and my WholesomeEndTexts mod.
- Port to 2.206, with big help from Cvolton, Fleym, and dankmeme. Big thanks to CatXus, Aktimoose, ninXout, and hiimjustin000 for helping out with testing!
- Several optimizations and fixes for visual errors.
- Add a toggle for custom endscreen messages.
- Add a button to toggle the EndLevelLayer's visibility.
- Replace `Hide "Hide Endscreen" Button` toggles (for both MHv8 and QOLMod) with a similar toggle for the same button from vanilla GD.
## v1.1.3
- Finally found a workaround past Cocos2D's tendency to ignore text alignment with multi-line quotes on Android. As such, quotes are no longer forcibly downscaled on Android. If your `bigFont.fnt` is somehow monospaced, that's on you.
## v1.1.2
- Futureproofing the mod against unexpected `m_fields` behavior between Geode versions. Also raised the minimum Geode version requirement.
## v1.1.1
- A single line swap to properly enforce text alignment.
## v1.1.0
- Added compat with GDMO's "Endscreen Info" option.
- Added option to hide QOLMod's "Hide Endscreen" buttons.
- Added "Max Scale" option.
- Note that if you have `relative`'s Custom Endscreen Text mod installed, you *will* need to copy and paste your custom end text messages from the txt file you've set for that mod over to this mod's config, as this mod *will* override those messages.
- *Technically*, this is the initial release for the Geode index, but there have been several notable changes between v1.0.0 and v1.1.0 that it's best to release a new version now to make sure everyone updates to this version.
## v1.0.0
- Initial release (on GitHub).
