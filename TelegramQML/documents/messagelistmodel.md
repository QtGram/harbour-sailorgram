# MessageListModel

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
|Component|<font color='#074885'>MessageListModel</font>|
|C++ class|<font color='#074885'>TelegramMessageListModel</font>|
|Inherits|<font color='#074885'>[AbstractEngineListModel](abstractenginelistmodel.md)</font>|
|Model|<font color='#074885'>Yes</font>|


### Required Properties

* <font color='#074885'><b>engine</b></font>: [Engine](engine.md)
* <font color='#074885'><b>currentPeer</b></font>: [InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md)


### Normal Properties

* <font color='#074885'><b>refreshing</b></font>: boolean (readOnly)
* <font color='#074885'><b>messageList</b></font>: list&lt;int&gt;
* <font color='#074885'><b>dateConvertorMethod</b></font>: function(){[code]}
* <font color='#074885'><b>key</b></font>: byte (readOnly)
* <font color='#074885'><b>typingUsers</b></font>: list&lt;variant&gt; (readOnly)
* <font color='#074885'><b>limit</b></font>: int
* <font color='#074885'><b>megagroup</b></font>: boolean (readOnly)
* <font color='#074885'><b>editable</b></font>: boolean (readOnly)
* <font color='#074885'><b>useCache</b></font>: boolean


### Methods

 * boolean <font color='#074885'><b>sendMessage</b></font>(string message, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup, function(){[code]} callback)
 * boolean <font color='#074885'><b>sendMessage</b></font>(string message, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup)
 * boolean <font color='#074885'><b>sendMessage</b></font>(string message, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo)
 * boolean <font color='#074885'><b>sendMessage</b></font>(string message)
 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup, function(){[code]} callback)
 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup)
 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo)
 * boolean <font color='#074885'><b>sendFile</b></font>(int type, string file)
 * void <font color='#074885'><b>deleteMessages</b></font>(list&lt;int&gt; msgs, function(){[code]} callback)
 * void <font color='#074885'><b>deleteMessages</b></font>(list&lt;int&gt; msgs)
 * void <font color='#074885'><b>forwardMessages</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) fromInputPeer, list&lt;int&gt; msgs, function(){[code]} callback)
 * void <font color='#074885'><b>forwardMessages</b></font>([InputPeer](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/inputpeer.md) fromInputPeer, list&lt;int&gt; msgs)
 * void <font color='#074885'><b>resendMessage</b></font>(int msgId, string newCaption, function(){[code]} callback)
 * void <font color='#074885'><b>resendMessage</b></font>(int msgId, string newCaption)
 * void <font color='#074885'><b>resendMessage</b></font>(int msgId)
 * void <font color='#074885'><b>sendSticker</b></font>([Document](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/document.md) doc, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup, function(){[code]} callback)
 * void <font color='#074885'><b>sendSticker</b></font>([Document](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/document.md) doc, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo, [ReplyMarkup](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/replymarkup.md) replyMarkup)
 * void <font color='#074885'><b>sendSticker</b></font>([Document](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/document.md) doc, [Message](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/message.md) replyTo)
 * void <font color='#074885'><b>sendSticker</b></font>([Document](https://github.com/Aseman-Land/libqtelegram-aseman-edition/blob/API51/telegram/documents/types/document.md) doc)
 * void <font color='#074885'><b>markAsRead</b></font>(function(){[code]} callback)
 * void <font color='#074885'><b>markAsRead</b></font>()
 * void <font color='#074885'><b>clearHistory</b></font>(boolean justClear, function(){[code]} callback)
 * void <font color='#074885'><b>clearHistory</b></font>(boolean justClear)
 * void <font color='#074885'><b>loadFrom</b></font>(int msgId)
 * void <font color='#074885'><b>loadBack</b></font>()
 * void <font color='#074885'><b>loadFront</b></font>()



### Enumerator


##### DataRoles

|Key|Value|
|---|-----|
|RoleMessageItem|256|
|RoleMediaItem|257|
|RoleServiceItem|258|
|RoleMarkupItem|259|
|RoleEntityList|260|
|RoleFromUserItem|261|
|RoleToUserItem|262|
|RoleToChatItem|263|
|RoleToPeerItem|264|
|RoleMessage|265|
|RoleDateTime|266|
|RoleDate|267|
|RoleUnread|268|
|RoleSent|269|
|RoleOut|270|
|RoleReplyMsgId|271|
|RoleReplyMessage|272|
|RoleReplyPeer|273|
|RoleForwardFromPeer|274|
|RoleForwardDate|275|
|RoleMessageType|276|
|RoleReplyType|277|
|RoleFileName|278|
|RoleFileMimeType|279|
|RoleFileTitle|280|
|RoleFilePerformer|281|
|RoleFileDuration|282|
|RoleFileIsVoice|283|
|RoleFileSize|284|
|RoleDownloadable|285|
|RoleUploading|286|
|RoleDownloading|287|
|RoleTransfaring|288|
|RoleTransfared|289|
|RoleTransfaredSize|290|
|RoleTotalSize|291|
|RoleFilePath|292|
|RoleThumbPath|293|


### Roles

 * model.<font color='#074885'>item</font>
 * model.<font color='#074885'>mediaItem</font>
 * model.<font color='#074885'>serviceItem</font>
 * model.<font color='#074885'>markupItem</font>
 * model.<font color='#074885'>entityList</font>
 * model.<font color='#074885'>fromUserItem</font>
 * model.<font color='#074885'>toUserItem</font>
 * model.<font color='#074885'>toChatItem</font>
 * model.<font color='#074885'>toPeerItem</font>
 * model.<font color='#074885'>message</font>
 * model.<font color='#074885'>dateTime</font>
 * model.<font color='#074885'>date</font>
 * model.<font color='#074885'>unread</font>
 * model.<font color='#074885'>sent</font>
 * model.<font color='#074885'>out</font>
 * model.<font color='#074885'>replyMsgId</font>
 * model.<font color='#074885'>replyMessage</font>
 * model.<font color='#074885'>replyPeer</font>
 * model.<font color='#074885'>forwardFromPeer</font>
 * model.<font color='#074885'>forwardDate</font>
 * model.<font color='#074885'>messageType</font>
 * model.<font color='#074885'>replyType</font>
 * model.<font color='#074885'>fileName</font>
 * model.<font color='#074885'>fileMimeType</font>
 * model.<font color='#074885'>fileTitle</font>
 * model.<font color='#074885'>filePerformer</font>
 * model.<font color='#074885'>fileDuration</font>
 * model.<font color='#074885'>fileIsVoice</font>
 * model.<font color='#074885'>fileSize</font>
 * model.<font color='#074885'>downloadable</font>
 * model.<font color='#074885'>uploading</font>
 * model.<font color='#074885'>downloading</font>
 * model.<font color='#074885'>transfaring</font>
 * model.<font color='#074885'>transfared</font>
 * model.<font color='#074885'>transfaredSize</font>
 * model.<font color='#074885'>totalSize</font>
 * model.<font color='#074885'>filePath</font>
 * model.<font color='#074885'>thumbPath</font>
