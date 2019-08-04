/* ======= STARTUP CONFIG ======= */
/* Needs both*/
//#define EMAIL "Your Default Email"
//#define PASSWORD "Your Default Password"

/* This Default Server and Channels needs stored Email and Password*/
//#define DEFUALT_SERVER "ServerName"
//#define DEFAULT_CHANNEL "ChannelName"

/* ======= MESSAGE CONFIG ======= */
/*
  You can adjust how your messages look like, read the following and implement these.
  =This values all should addup to a string=

  Variables
  message : Message Object, contains message data
  ├ message.m_id : Message id in Snowflake Object
  ├ message.m_channelId : Channel id in Snowflake Object
  ├ message.m_guildId : Guild id in Snowflake Object
  ├ message.author() : User Object
  ├ messate.timestamp() : raw timestamp in ISO6801 form
  └ message.content() : raw content in std::string
  (Check Message.h file for more detail)

  Functions
  ├ formatTime(message.timestamp(), "C++ Time Format") : returns timestamp in format that have provided in 2nd parameter
  ├ formatAuthor(message) : returns formated author name, nickname gets priority
  └ formatContent(message) : return formated content

  Needs Semicolon at the end
 */
#define MESSAGE_FORMAT "\033[36m" + formatTime(message.timestamp(), "%H:%M") + "\033[0m" + " | " + formatAuthor(message)+ " | " + formatContent(message);

/* How an attachment only message would look like */
#define ATTACHMENT_LABEL "\033[104m(Attachment)\033[0m"

/* Escape Sequence for mention color */
#define MENTION_COLOR "\033[101m\033[30m"

/*
   Disables DeEmojizing process, if your terminal does not have full emoji font installed,
   Emjis will appear as squarebox.
   Not Recommanded
*/
//#define DisableDeEmojize

/* Configured size for Username */
#define USERNAME_SIZE 25

/* ======= GENERAL CONFIG ======= */
/* Length of dividers that is printing after each commands */
#define COMMAND_DIVIDER_LENGTH 40

/* Char of dividers that is printing after each commands */
#define COMMAND_DIVIDER_CHAR '-'

/*
   How is your prompt look like

   Prefixes
   %s : Server Name
   %c : channel Name

   currently "%s%c"" works best
 */
#define PROMPT "[%s%c]>"

/* Divider Char that divides differnet options*/
#define PROMPT_DIVIDER "/"




