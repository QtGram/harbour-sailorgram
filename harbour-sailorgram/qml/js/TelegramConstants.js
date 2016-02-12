.pragma library

/* FileHandler Constants */
var typeProgressEmpty = 0;
var typeProgressUpload = 1;
var typeProgressDownload = 2;

/* Message Constants */
var typeMessageEmpty = 0x83e5de54;
var typeMessageService = 0x1d86f70e;
var typeMessage = 0xa7ab1991;

/* Message Action Constants */
var typeMessageActionEmpty = 0xb6aef7b0;
var typeMessageActionChatDeletePhoto = 0x95e3fbef;
var typeMessageActionChatCreate = 0xa6638b9a;
var typeMessageActionChatEditTitle = 0xb5a1ce5a;
var typeMessageActionChatEditPhoto = 0x7fcb13a8;
var typeMessageActionGeoChatCreate = 0x6f038ebc;
var typeMessageActionChatDeleteUser = 0xb2ae9b0c;
var typeMessageActionChatAddUser = 0x5e3cfc4b;
var typeMessageActionChatJoinedByLink = 0xf89cf5e8;

/* Message Media Constants */
var typeMessageMediaEmpty = 0x3ded6320;
var typeMessageMediaPhoto = 0x3d8ce53d;
var typeMessageMediaVideo = 0x5bcf1675;
var typeMessageMediaGeo = 0x56e0d474;
var typeMessageMediaContact = 0x5e7d2f39;
var typeMessageMediaUnsupported = 0x9f84f49e;
var typeMessageMediaDocument = 0x2fda2204;
var typeMessageMediaAudio = 0xc6b68300;
var typeMessageMediaWebPage = 0xa32dd600;
var typeMessageMediaVenue = 0x7912b71;

/* Document Media Attributes Constants */
var typeDocumentAttributeImageSize = 0x6c37c15c;
var typeDocumentAttributeAnimated = 0x11b58939;
var typeDocumentAttributeSticker = 0x3a556302;
var typeDocumentAttributeVideo = 0x5910cccb;
var typeDocumentAttributeAudio = 0x51448e5;
var typeDocumentAttributeFilename = 0x15590068;

/* Secret Chat Constants */
var typeEncryptedChatWaiting = 0x3bf703dc;
var typeEncryptedChatRequested = 0xc878527e;
var typeEncryptedChatEmpty = 0xab7ec0a0;
var typeEncryptedChatDiscarded = 0x13d6dd27;
var typeEncryptedChat = 0xfa56ce36;

/* Peer Constants */
var typePeerUser = 0x9db1bc6d;
var typePeerChat = 0xbad0e5bb;

/* User Constants */
var typeUserStatusEmpty = 0x9d05049;
var typeUserStatusOnline = 0xedb93949;
var typeUserStatusOffline = 0x8c703f;
var typeUserStatusRecently = 0xe26f42f1;
var typeUserStatusLastWeek = 0x7bf09fc;
var typeUserStatusLastMonth = 0x77ebc742;

/* User Type Constants */
var typeUserSelf = 0x1c60e608;
var typeUserContact = 0xcab35e18;
var typeUserDeleted = 0xd6016d7a;
var typeUserForeign = 0x75cf7a8;
var typeUserEmpty = 0x200250ba;
var typeUserRequest = 0xd9ccc4ef;
