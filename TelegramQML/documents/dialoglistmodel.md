# DialogListModel

 * [Component details](#component-details)
 * [Required Properties](#required-properties)
 * [Normal Properties](#normal-properties)
 * [Enumerator](#enumerator)
 * [Methods](#methods)
 * [Roles](#roles)


### Component details:

|Detail|Value|
|------|-----|
|Import|TelegramQml 2.0|
|Component|<font color='#074885'>DialogListModel</font>|
|C++ class|<font color='#074885'>TelegramDialogListModel</font>|
|Inherits|<font color='#074885'>[AbstractEngineListModel](abstractenginelistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)


### Normal Properties

* <font color='#074885'><b>visibility</b></font>: int
* <font color='#074885'><b>sortFlag</b></font>: list&lt;int&gt;
* <font color='#074885'><b>filter</b></font>: string
* <font color='#074885'><b>dateConvertorMethod</b></font>: function(){[code]}
* <font color='#074885'><b>messageTextMethod</b></font>: function(){[code]}
* <font color='#074885'><b>refreshing</b></font>: boolean (readOnly)
* <font color='#074885'><b>useCache</b></font>: boolean
* <font color='#074885'><b>categories</b></font>: map


### Methods

 * int <font color='#074885'><b>indexOf</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) peer)
 * void <font color='#074885'><b>clearHistory</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) peer, boolean justClear, function(){[code]} callback)
 * void <font color='#074885'><b>clearHistory</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) peer, boolean justClear)
 * void <font color='#074885'><b>clearHistory</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) peer)



### Enumerator


##### VisibilityFlags

|Key|Value|
|---|-----|
|VisibilityEmptyDialogs|1|
|VisibilityDeletedDialogs|2|
|VisibilityOnlineUsersOnly|4|
|VisibilityContacts|8|
|VisibilityNonContacts|16|
|VisibilityBots|32|
|VisibilityUsers|24|
|VisibilityChats|64|
|VisibilityChannels|128|
|VisibilitySecretChats|256|
|VisibilityAll|507|

##### SortFlag

|Key|Value|
|---|-----|
|SortByType|0|
|SortByName|1|
|SortByDate|2|
|SortByUnreads|3|
|SortByOnline|4|
|SortByCategories|5|

##### DataRoles

|Key|Value|
|---|-----|
|RoleDialogItem|256|
|RoleChatItem|257|
|RoleUserItem|258|
|RoleTopMessageItem|259|
|RolePeerItem|260|
|RolePeerHex|261|
|RoleIsSecretChat|262|
|RoleSecretChatState|263|
|RoleName|264|
|RoleMessageDate|265|
|RoleMessageUnread|266|
|RoleMessage|267|
|RoleMessageOut|268|
|RoleMessageType|269|
|RoleMessageUser|270|
|RoleLastOnline|271|
|RoleIsOnline|272|
|RoleStatus|273|
|RoleStatusText|274|
|RoleTyping|275|
|RoleUnreadCount|276|
|RoleMute|277|
|RoleCategory|278|

##### SecretChatState

|Key|Value|
|---|-----|
|SecretChatStateInit|0|
|SecretChatStateRequested|1|
|SecretChatStateAccepted|2|
|SecretChatStateCancelled|3|


### Roles

 * model.<font color='#074885'>dialog</font>
 * model.<font color='#074885'>chat</font>
 * model.<font color='#074885'>user</font>
 * model.<font color='#074885'>topMessage</font>
 * model.<font color='#074885'>peer</font>
 * model.<font color='#074885'>peerHex</font>
 * model.<font color='#074885'>isSecretChat</font>
 * model.<font color='#074885'>secretChatState</font>
 * model.<font color='#074885'>title</font>
 * model.<font color='#074885'>messageDate</font>
 * model.<font color='#074885'>messageUnread</font>
 * model.<font color='#074885'>message</font>
 * model.<font color='#074885'>messageOut</font>
 * model.<font color='#074885'>messageType</font>
 * model.<font color='#074885'>messageUser</font>
 * model.<font color='#074885'>lastOnline</font>
 * model.<font color='#074885'>isOnline</font>
 * model.<font color='#074885'>status</font>
 * model.<font color='#074885'>statusText</font>
 * model.<font color='#074885'>typing</font>
 * model.<font color='#074885'>unreadCount</font>
 * model.<font color='#074885'>mute</font>
 * model.<font color='#074885'>category</font>
