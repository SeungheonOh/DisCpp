# DisCpp
Are you looking for a keyboard centric discord client, that does not require you to use pointing device, the mouse, at all?  Than, you found the right one! DisCpp is a Cli Discord client, written in C++! 

# Preperations
DisCpp is made out of few external libraries such as Jsoncpp, libcurl, curlpp, and more.
In linux, fortunatly, your package manager will take all the cares.
Here's the list of packages that is reqired to compile DisCpp :
```
boost_system ( 1.69 or lower)
websocketpp
ssl
crypto
jsoncpp
libcurl
curlpp
```
than just clone the repo, with **--recurse-submodules** this command. The command is for cloning EmojiTools, the submoudel, as well.

# Installation
Just type ```make``` and it will generate ```DisCpp``` execuatble, and you can put it on wherever you wish!

# Commands
| :login | Login with your discord account |
| :login ```email``` ```password``` | Login with your discord account |
| :servers | List your servers |
| :servers ```servername``` | access to the server |
| :channels | List your channel, requires setting server before |
| :channels ```channelname``` | access to the channel, requires setting server before |
| :status ```status``` | change the name of game you are playing |
| :exit | terminates the program |


# Demo
(https://www.reddit.com/r/unixporn/comments/cfd64p/oc_new_cli_discord_client_discpp_that_is_purely/?utm_source=share&utm_medium=web2x)
