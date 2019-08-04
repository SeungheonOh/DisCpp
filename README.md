# DisCpp
Are you looking for a keyboard centric discord client, that does not require you to use pointing device, the mouse, at all?  Then, you found the right one! DisCpp is a CLI Discord client, written in C++! 

# Preparations
DisCpp uses a few external libraries such as jsoncpp, libcurl, curlpp, and more.
In Linux, fortunately, your package manager will take all the care.
Here's the list of packages that are reqired to compile DisCpp :
```
boost_system ( 1.69 or lower)
websocketpp
ssl
crypto
jsoncpp
libcurl
curlpp
```
Then, just clone the repo using git with the **--recurse-submodules** option, which is for cloning EmojiTools, the submodule, as well.

# Installation
```mkdir build && cd build && cmake .. && make```
If it's successful, there will be an execuable name ```DisCpp``` in ```build``` folder.

# Configuraions
DisCpp supports external config file, ```config.h```, user can set default login account or appearences.
After editing ```config.h``` file, in ```build``` folder type ```make clean all``` will apply new settings and create a new DisCpp execuable.

Further deatils are provided in ```config.h``` file.

# Commands
| Commands                          | Purpose                                               |
|-----------------------------------|-------------------------------------------------------|
| :login                            | Login with your discord account                       |
| :login ```email``` ```password``` | Login with your discord account                       |
| :servers                          | List your servers                                     |
| :servers ```Server Name```        | access to the server                                  |
| :servers DM                       | access to DM channels                                 |
| :channels                         | List your channel, requires setting server before     |
| :channels ```Channel Name```      | access to the channel, requires setting server before |
| :status ```status```              | change the name of game that you are playing          |
| :edit ```message```         | edit the last message that sent by you |
| :delete ```number```     | deletes ```number```th message from newest message that sent by you  |
| :reload         | reloads the message screen |
| :clear                            | equivlent to terminal comand "clear"                  |
| :exit                             | terminates the program                                |


# [Demo](https://www.reddit.com/r/unixporn/comments/cfd64p/oc_new_cli_discord_client_discpp_that_is_purely/?utm_source=share&utm_medium=web2x)
