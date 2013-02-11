# Japanese language file.
#
# (C) 2003-2013 Anope Team
# Contact us at team@anope.org
#   
# Please read COPYING and README for further details.
#     
# Based on the original code of Epona by Lara.
# Based on the original code of Services by Andy Church. 
#
# When translating this file to another language, keep in mind that the
# order of parameters for sprintf() is fixed in the source code, so your
# messages need to take the same parameters in the same order as the
# English messages do.  (Obviously, this doesn't hold for the strftime()
# format lines immediately below.)  If you can't get a natural translation
# of a message without changing the order of the parameters, let us know
# (team@anope.org) which message is causing a problem and I'll see
# what I can do.
#
# In help messages, "%S" (capital S, not lowercase) refers to the name of
# the service sending the message; for example, in NickServ help messages,
# "%S" is replaced by "NickServ" (or whatever it is renamed to in
# services.conf).  The %S's do not count as sprintf() parameters, so they can be
# rearranged, removed, or added as necessary.
#
# Also in help messages, please try to limit line lengths to 60 characters
# of text (not including the leading tab).  This length was chosen because
# it does not cause line wrap under default settings on most current IRC
# clients.  Remember that format characters (control-B, control-_) are not
# included in that 60-character limit (since they don't show on the user's
# screen).  Also remember that format specifiers (%S, etc.) will expand
# when displayed, so remember to take this into account; you can assume
# that the length of a pseudoclient name (%S replacement) will be eight
# characters, so reduce the maximum line length by 6 for every %S on a
# line.
#
# Finally, remember to put a tab at the beginning of every line of text
# (including empty lines).  This has to be a tab, not spaces.

###########################################################################
#
# Name of this language
#
###########################################################################

# For languages other than English, this string should have the following
# format:
#     language-name-in-language (language-name-in-English)
# For example, "Espa�l (Spanish)" or "Fran�is (French)".

LANG_NAME
	日本語 UTF-8 (Japanese UTF-8)

###########################################################################
#
# General messages
#
###########################################################################

# strftime() format strings.  man 3 strftime for information on the
# meanings of the format specifiers.  Short version:
#	%a = weekday name (short)	%H = hour
#	%b = month name (short)		%M = minute
#	%d = day of month		%S = second
#	%Y = year			%Z = time zone

# This is used as the format string for strftime() for a date and time
# together.
STRFTIME_DATE_TIME_FORMAT
	%Y 年 %b 月 %d 日(%a) %H 時 %M 分 %S 秒 %Z
# This is used as the format string for strftime() for a date alone in long
# format (English: including weekday).
STRFTIME_LONG_DATE_FORMAT
	%Y 年 %b 月 %d 日(%a)
# This is used as the format string for strftime() for a date alone in
# short format (English: without weekday).
STRFTIME_SHORT_DATE_FORMAT
	%Y/%b/%d

# These tell our strftime() what the names of months and days are.  If you
# don't use %a, %A, %b, or %B in your strftime() strings above, you can
# leave these empty.  However, if you enter names, they MUST stay in order,
# one per line, and the list MUST be complete!

# %a
STRFTIME_DAYS_SHORT
	日
	月
	火
	水
	木
	金
	土
# %A
STRFTIME_DAYS_LONG
	日曜日
	月曜日
	火曜日
	水曜日
	木曜日
	金曜日
	土曜日
# %b
STRFTIME_MONTHS_SHORT
	1
	2
	3
	4
	5
	6
	7
	8
	9
	10
	11
	12
# %B
STRFTIME_MONTHS_LONG
	1 月
	2 月
	3 月
	4 月
	5 月
	6 月
	7 月
	8 月
	9 月
	10 月
	11 月
	12 月


# This is used in ChanServ/NickServ INFO displays.
COMMA_SPACE
	、


# さまざまなエラーメッセージ。
USER_RECORD_NOT_FOUND
	内部エラー - リクエスト処理不能。
UNKNOWN_COMMAND
	不明なコマンド: %s。
UNKNOWN_COMMAND_HELP
	不明なコマンド: %s。"%R%s HELP" としてヘルプを参照して下さい。
SYNTAX_ERROR
	構文: %s
MORE_INFO
	%R%s HELP %s として詳細な情報を得てください。
NO_HELP_AVAILABLE
	%s についてのヘルプは存在しません。
OBSOLETE_COMMAND
	このコマンドは廃止されました。代わりに %s をお使いください。

BAD_USERHOST_MASK
	マスクは user@host の形式でなければなりません。
BAD_EXPIRY_TIME
	無効な失効期限です。
USERHOST_MASK_TOO_WIDE
	%s の範囲は広すぎます。もっと具体的なマスクを使用してください。

SERVICE_OFFLINE
	%s は現在オフラインです。
READ_ONLY_MODE
	通知: サービスは現在読み込み専用モードです。変更は保存されません!
PASSWORD_INCORRECT
	パスワードが間違っています。
INVALID_TARGET
	"/msg %s" はもうサポートされません。代わりに "/msg %s@%s" または "/%s" を使用してください。

# "Access denied" (アクセス否認) と "Permission denied" (許可否認) の違いはなんでしょうか?
# 非常に小さなことです、意味の損失なく安全に同じメッセージとみなすことができます。
# 区別するならば、"Access denied" (アクセス否認) はより一般的に用いられるものです、
# 例えば、非運営者が OperServ とアクセスすると"Access denied"が得られます、
# 一方、非サービス管理者が NickServ SASET NOEXPIRE を使おうとすると
# "Permission denied" が得られます。
ACCESS_DENIED
	アクセス権がありません。
PERMISSION_DENIED
	許可がありません。
RAW_DISABLED
	RAW オプションは無効にされています。もし使う必要があるならば、サービス設定の DisableRaw 指示文を設定してください。

MORE_OBSCURE_PASSWORD
	もっと分かり難いパスワードで再度お試しください。パスワードは少なくとも 5 文字の長さで、簡単に推測できるもの (例: あなたの本名やニックネーム) にすることなく、空白やタブ文字を含まないようにもしてください。
PASSWORD_TOO_LONG
	パスワードが長すぎます。短いパスワードを試してください。

NICK_NOT_REGISTERED
	あなたのニックネームは登録されていません。
NICK_NOT_REGISTERED_HELP
	あなたのニックネームは登録されていません。ニックネームの登録については %R%s HELP と入力して情報を得てください。
NICK_X_IS_SERVICES
	ニックネーム %s はこのネットワークサービスの一部です。
NICK_X_NOT_REGISTERED
	ニックネーム %s は登録されていません。
NICK_X_IN_USE
	ニックネーム %s は使用中です。
NICK_X_NOT_IN_USE
	ニックネーム %s は使用中ではありません。
NICK_X_NOT_ON_CHAN
	%s はチャンネル %s では使われていません。
NICK_X_FORBIDDEN
	ニックネーム %s はおそらく登録も使用もされていません。
NICK_X_FORBIDDEN_OPER
	ニックネーム %s は %s によって禁止されています:
	%s
NICK_X_ILLEGAL
	ニックネーム %s は不法ニックネームであり使うことはできません。
NICK_X_TRUNCATED
	ニックネーム %s は %d 文字に切り詰められました。
NICK_X_SUSPENDED
	ニックネーム %s は現在保留されています。
CHAN_X_NOT_REGISTERED
	チャンネル %s は登録されていません。
CHAN_X_NOT_IN_USE
	チャンネル %s は存在しません。
CHAN_X_FORBIDDEN
	チャンネル %s はおそらく登録も使用もされていません。
CHAN_X_FORBIDDEN_OPER
	チャンネル %s は %s によって禁止されています:
	%s
CHAN_X_SUSPENDED
	      保留: [%s] %s
NICK_IDENTIFY_REQUIRED
	このコマンドはパスワード認証が必要です。
	%R%s IDENTIFY password と入力してからやり直してください。
CHAN_IDENTIFY_REQUIRED
	このコマンドはパスワード認証が必要です。
	%R%s IDENTIFY %s password と入力してからやり直してください。
	
MAIL_DISABLED
	サービスは現在メール送信できないよう設定されています。
MAIL_INVALID
	%s への電子メールは無効です。
MAIL_X_INVALID
	%s は無効な電子メールアドレスです。
MAIL_LATER
	現在メール送信できません。もう少し後にまたやり直してください。
MAIL_DELAYED
	%d秒待ってからやり直してください。
	
NO_REASON
	理由なし
UNKNOWN
	<不明>

# Duration system
DURATION_DAY
	1 日間
DURATION_DAYS
	%d 日間
DURATION_HOUR
	1 時間
DURATION_HOURS
	%d 時間
DURATION_MINUTE
	1 分間
DURATION_MINUTES
	%d 分間
DURATION_SECOND
	1 秒間
DURATION_SECONDS
	%d 秒間

# Human readable expiration
NO_EXPIRE
	失効しない
EXPIRES_SOON
	次回のデータベース更新にて失効
EXPIRES_M
	あと %d 分で失効
EXPIRES_1M
	あと %d 分で失効
EXPIRES_HM
	あと %d 時間 %d 分で失効
EXPIRES_H1M
	あと %d 時間 %d 分で失効
EXPIRES_1HM
	あと %d 時間 %d 分で失効
EXPIRES_1H1M
	あと %d 時間 %d 分で失効
EXPIRES_D
	あと %d 日で失効
EXPIRES_1D
	あと %d 日で失効

# Generic Footer message
END_OF_ANY_LIST
	%s リスト終端.

# Generic List error messages
LIST_INCORRECT_RANGE
	無効な範囲が指定されました。正しい構文は #from-to の形です。
CS_LIST_INCORRECT_RANGE
	# から始まるチャンネルを検索するには、# を前置しないでください。
	(つまり、#anope の代わりに anope のように指定してください。)

# Generic help limited to messages
HELP_LIMIT_SERV_OPER
	
	サービスオペレータの権限が必要です。
HELP_LIMIT_SERV_ADMIN
	
	サービス管理者の権限が必要です。
HELP_LIMIT_SERV_ROOT
	
	サービスルートの権限が必要です。
HELP_LIMIT_IRC_OPER
	
	IRC オペレータの権限が必要です。
HELP_LIMIT_HOST_SETTER
	
	Host Setters の権限が必要です。
HELP_LIMIT_HOST_REMOVER
	
	Host Removers の権限が必要です。


###########################################################################
#
# NickServ メッセージ
#
###########################################################################

# 自動メッセージ
NICK_IS_REGISTERED
	このニックネームは既に登録されています。他のニックネームにしてください。
	(もしこのニックネームはあなた自身のものなら %R%s IDENTIFY password と入力してください。)
NICK_IS_SECURE
	このニックネームは他のユーザに登録されていて保護されています。
	もしこれはあなた自身のニックネームなら %R%s IDENTIFY password と入力してくさだい。
	そうでない場合は他のニックネームにしてください。
NICK_MAY_NOT_BE_USED
	指定されたニックネームはお使いになれません。他のニックネームにしてください。
FORCENICKCHANGE_IN_1_MINUTE
	1 分以内にニックネームを変更しない場合、あなたのニックネームは自動的に変更されます。
FORCENICKCHANGE_IN_20_SECONDS
	20 秒以内にニックネームを変更しない場合、あなたのニックネームは自動的に変更されます。
FORCENICKCHANGE_NOW
	このニックネームは登録済みのため、使用出来ません。
FORCENICKCHANGE_CHANGING
	あなたのニックネームは %s に変更されます。

# REGISTER responses
NICK_REGISTER_SYNTAX
	REGISTER パスワード [メールアドレス]
NICK_REGISTER_SYNTAX_EMAIL
	REGISTER パスワード メールアドレス
NICK_REGISTRATION_DISABLED
	ご迷惑をおかけしますが、現在ニックネームの登録サービスは一時的に停止させて頂いております。
NICK_REGISTRATION_FAILED
	ご迷惑をおかけしますが、ニックネームの登録に失敗しました。
NICK_REG_PLEASE_WAIT
	REGISTER コマンドを再度使う前に %d 秒待って頂かなければなりません。
NICK_CANNOT_BE_REGISTERED
	%s というニックネームはお使いになれません。
NICK_ALREADY_REGISTERED
	%s というニックネームは既に登録されています。
NICK_REGISTERED
	%s というニックネームは %s のアカウントに登録されています。
NICK_REGISTERED_NO_MASK
	%s というニックネームは既に登録されています。
NICK_PASSWORD_IS
	あなたのパスワードは %s に設定されています。覚えておいてください。
NICK_REG_DELAY
	ニックネームを登録するには、最低 %d 秒間そのニックネームを使わなければなりません。

# GROUP responses
NICK_GROUP_SYNTAX
	GROUP 対象ニックネーム パスワード
NICK_GROUP_DISABLED
	ご迷惑おかけしますが、ニックネームのグループまとめサービスは一時的に停止させて頂いております。
NICK_GROUP_FAILED
	グループ登録に失敗しました。
NICK_GROUP_PLEASE_WAIT
	再度 GROUP コマンドを使うには %d 秒間待っていただく必要があります。
NICK_GROUP_CHANGE_DISABLED
	指定されたニックネームは既に登録されています。まず、%R%s DROP と入力して登録の解除を行ってください。
NICK_GROUP_SAME
	あなたは既に %s というグループに入っています。
NICK_GROUP_TOO_MANY
	登録数の上限に達したため、グループ名 %s にニックネームを登録できません。
	登録済みのニックネームから不要なものを削除してください。
	%R%s HELP GLIST と %R%s HELP DROP というコマンドを入力することで詳しい説明を読むことが出来ます。
NICK_GROUP_JOINED
	%sというグループのメンバーになりました。

# IDENTIFY responses
NICK_IDENTIFY_SYNTAX
	IDENTIFY パスワード
NICK_IDENTIFY_FAILED
	認証に失敗しました。
NICK_IDENTIFY_SUCCEEDED
	パスワードは承認されました。あなたは認証されています。
NICK_IDENTIFY_EMAIL_REQUIRED
	ニックネームに対するパスワードを忘れた際に
	パスワードの再発行を行うための連絡先として
	電子メールアドレスを入力して頂きます。
NICK_IDENTIFY_EMAIL_HOWTO
	あなたの電子メールアドレスを設定するには %R%S SET EMAIL e-mail
	と入力してください。このアドレスは個人情報として扱われて、
	第三者に渡すことはありません。
NICK_ALREADY_IDENTIFIED
	あなたは既に認証済みです。

# UPDATE responses
NICK_UPDATE_SUCCESS
	memo、vhost、chmode、flag の設定を更新しました。
	
# LOGOUT responses
NICK_LOGOUT_SYNTAX
	LOGOUT
NICK_LOGOUT_SUCCEEDED
	ニックネームはログアウトされました。
NICK_LOGOUT_X_SUCCEEDED
	ニックネーム %s はログアウトされました。
NICK_LOGOUT_SERVICESADMIN
	%s は管理者なので、ログアウトさせることは出来ません。

# DROP (ニックネーム登録解除) についての応答
NICK_DROP_DISABLED
	ご迷惑をおかけしますが、ニックネームの登録解除サービスは一時的に停止させていただいております。
NICK_DROPPED
	ニックネームの登録は解除されました。
NICK_X_DROPPED
	ニックネーム %s の登録は解除されました。

# SET (ニックネームの設定) についての応答
NICK_SET_SYNTAX
	SET 設定項目 パラメータ
NICK_SET_SERVADMIN_SYNTAX
	SET [ニックネーム] オプション パラメータ
NICK_SET_DISABLED
	ご迷惑をおかけしますが、ニックネームオプションの設定は一時無効にされます。
NICK_SET_UNKNOWN_OPTION
	未定義の SET オプション: %s。
NICK_SET_OPTION_DISABLED
	%s オプションは、このネットワークに設定できません。

# SET DISPLAY responses
NICK_SET_DISPLAY_INVALID
	新しいディスプレイ名はニックネームグループ内にあるニックネームでなければいけません!
NICK_SET_DISPLAY_CHANGED
	ディスプレイ名は %s に変更されました。

# SET PASSWORD responses
NICK_SET_PASSWORD_FAILED
	パスワードの変更に失敗しました。
NICK_SET_PASSWORD_CHANGED
	パスワードを変更しました。
NICK_SET_PASSWORD_CHANGED_TO
	パスワードを %s に変更しました。

# SET LANGUAGE responses
NICK_SET_LANGUAGE_SYNTAX
	SET LANGUAGE 言語番号
NICK_SET_LANGUAGE_UNKNOWN
	未知の言語番号 %d を指定しました。言語の一覧を見るには %R%s HELP SET LANGUAGE と入力してください。
NICK_SET_LANGUAGE_CHANGED
	言語を日本語に切り替えました。

# SET URL responses
NICK_SET_URL_CHANGED
	URL を %s に変更しました。
NICK_SET_URL_UNSET
	URL 設定を外しました。

# SET EMAIL responses
NICK_SET_EMAIL_CHANGED
	電子メールアドレスを %s に変更しました。
NICK_SET_EMAIL_UNSET
	電子メールアドレスの設定を外しました。
NICK_SET_EMAIL_UNSET_IMPOSSIBLE
	本ネットワークでは電子メールの設定を外すことは出来ません。
	
# SET ICQ responses
NICK_SET_ICQ_CHANGED
	ICQ 番号を %s に変更しました。
NICK_SET_ICQ_UNSET
	ICQ 番号の設定を外しました。
NICK_SET_ICQ_INVALID
	%s は有効な ICQ 番号ではありません。
	
# SET GREET responses
NICK_SET_GREET_CHANGED
	歓迎メッセージを %s に変更しました。
NICK_SET_GREET_UNSET
	歓迎メッセージの設定を外しました。

# SET PROTECT responses
NICK_SET_KILL_SYNTAX
	SET KILL {ON | QUICK | OFF}
NICK_SET_KILL_IMMED_SYNTAX
	SET KILL {ON | QUICK | IMMED | OFF}
NICK_SET_KILL_ON
	ニックネーム保護を有効 ON にしました。
NICK_SET_KILL_QUICK
	ニックネーム保護を短期限つきで有効 ON にしました。
NICK_SET_KILL_IMMED
	ニックネーム保護を期限なしで有効 ON にしました。
NICK_SET_KILL_IMMED_DISABLED
	このネットワークでは、IMMED の設定は無効にされています。
NICK_SET_KILL_OFF
	ニックネーム保護を無効 OFF にしました。

# SET SECURE responses
NICK_SET_SECURE_SYNTAX
	SET SECURE {ON | OFF}
NICK_SET_SECURE_ON
	ニックネームセキュリティ機能を有効 ON にしました。
NICK_SET_SECURE_OFF
	ニックネームセキュリティ機能を無効 OFF にしました。

# SET PRIVATE responses
NICK_SET_PRIVATE_SYNTAX
	SET PRIVATE {ON | OFF}
NICK_SET_PRIVATE_ON
	ニックネームプライベート機能を有効 ON にしました。
NICK_SET_PRIVATE_OFF
	ニックネームプライベート機能を無効 OFF にしました。

# SET HIDE responses
NICK_SET_HIDE_SYNTAX
	SET HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SET_HIDE_EMAIL_ON
	あなたの電子メールアドレスを %s INFO コマンドで表示しないように設定しました。
NICK_SET_HIDE_EMAIL_OFF
	あなたの電子メールアドレスを %s INFO コマンドで表示するように設定しました。
NICK_SET_HIDE_MASK_ON
	あなたのホストマスクを %s INFO コマンドで表示しないように設定しました。
NICK_SET_HIDE_MASK_OFF
	あなたのホストマスクを %s INFO コマンドで表示するように設定しました。
NICK_SET_HIDE_QUIT_ON
	最も最近 IRC を終了した時の終了メッセージを %s INFO コマンドで表示しないように設定しました。
NICK_SET_HIDE_QUIT_OFF
	最も最近 IRC を終了した時の終了メッセージを %s INFO コマンドで表示するように設定しました。
NICK_SET_HIDE_STATUS_ON
	サービスへのアクセス状況を %s INFO コマンドで表示しないように設定しました。
NICK_SET_HIDE_STATUS_OFF
	サービスへのアクセス状況を %s INFO コマンドで表示するように設定しました。
	
# SET MSG responses
NICK_SET_MSG_SYNTAX
	SET MSG {ON | OFF}
NICK_SET_MSG_ON
	これから各サービスは連絡手段としてメッセージを使います。
NICK_SET_MSG_OFF
	これから各サービスは連絡手段として告知を使います。

# SET AUTOOP responses
NICK_SET_AUTOOP_SYNTAX
	SET AUTOOP {ON | OFF}
NICK_SET_AUTOOP_ON
	サービスは自動的に管理者権を与えるように設定しました。
NICK_SET_AUTOOP_OFF
	サービスは自動的に管理者権を与えないように設定しました。


# SASET responses
NICK_SASET_SYNTAX
	SASET ニックネーム オプション パラメータ
NICK_SASET_DISABLED
	ご迷惑をお掛けします。ニックネームオプションは一時的に無効になります。
NICK_SASET_UNKNOWN_OPTION
	未定義の SASET オプション: %s。
NICK_SASET_BAD_NICK
	ニックネーム %s は、登録されませんでした。
NICK_SASET_OPTION_DISABLED
	%s オプションはこのネットワークには設定できません。

# SASET DISPLAY responses
NICK_SASET_DISPLAY_INVALID
	%s の新しいディスプレイ名はグループ内にあるニックネームである必要があります。
NICK_SASET_DISPLAY_CHANGED
	ディスプレイ名は %s に変更されました。

# SASET PASSWORD responses
NICK_SASET_PASSWORD_FAILED
	ご迷惑をお掛けします。%s のパスワードを変更することは出来ませんでした。
NICK_SASET_PASSWORD_CHANGED
	%s のパスワードが変更されました。
NICK_SASET_PASSWORD_CHANGED_TO
	%s のパスワードは %s に変更されました。

# SASET URL responses
NICK_SASET_URL_CHANGED
	%s の URL を %s に変更しました。
NICK_SASET_URL_UNSET
	URL %s を解除しました。

# SASET EMAIL responses
NICK_SASET_EMAIL_CHANGED
	%s のメールアドレスを %s に変更しました。
NICK_SASET_EMAIL_UNSET
	%s のメールアドレスを解除しました。
NICK_SASET_EMAIL_UNSET_IMPOSSIBLE
	このネットワークではメールアドレスを解除することは出来ません。
	
# SASET ICQ responses
NICK_SASET_ICQ_CHANGED
	%s の ICQ 番号を %s に設定しました。
NICK_SASET_ICQ_UNSET
	%s の ICQ 番号を解除しました。
NICK_SASET_ICQ_INVALID
	%s は有効な値ではありません。
	
# SASET GREET responses
NICK_SASET_GREET_CHANGED
	%s の歓迎メッセージを %s に変更しました。
NICK_SASET_GREET_UNSET
	%s の歓迎メッセージを解除しました。

# SASET PROTECT responses
NICK_SASET_KILL_SYNTAX
	SASET ニックネーム KILL {ON | QUICK | OFF}
NICK_SASET_KILL_IMMED_SYNTAX
	SASET ニックネーム KILL {ON | QUICK | IMMED | OFF}
NICK_SASET_KILL_ON
	現在 %s への保護は ON になっています。
NICK_SASET_KILL_QUICK
	現在 %s への保護と、遅延の緩和が ON になっています。
NICK_SASET_KILL_IMMED
	現在 %s への保護と、遅延なしが ON になっています。
NICK_SASET_KILL_IMMED_DISABLED
	このネットワークでは IMMED オプションを利用することは出来ません。
NICK_SASET_KILL_OFF
	現在 %s への保護は OFF になっています。

# SASET SECURE responses
NICK_SASET_SECURE_SYNTAX
	SASET ニックネーム SECURE {ON | OFF}
NICK_SASET_SECURE_ON
	現在 %s へのセキュアオプションは ON になっています。
NICK_SASET_SECURE_OFF
	現在 %s へのセキュアオプションは OFF になっています。

# SASET PRIVATE responses
NICK_SASET_PRIVATE_SYNTAX
	SASET ニックネーム PRIVATE {ON | OFF}
NICK_SASET_PRIVATE_ON
	現在 %s へのプライベートオプションは ON になっています。
NICK_SASET_PRIVATE_OFF
	現在 %s へのプライベートオプションは OFF になっています。

# SASET HIDE responses
NICK_SASET_HIDE_SYNTAX
	SASET ニックネーム HIDE {EMAIL | USERMASK | QUIT} {ON | OFF}
NICK_SASET_HIDE_EMAIL_ON
	現在 %s のメールアドレスは %s INFO 情報に表示しないように設定されています。
NICK_SASET_HIDE_EMAIL_OFF
	現在 %s のメールアドレスは %s INFO 情報に表示するように設定されています。
NICK_SASET_HIDE_MASK_ON
	現在 %s のホストマスクは %s INFO 情報に表示しないように設定されています。
NICK_SASET_HIDE_MASK_OFF
	現在 %s のホストマスクは %s INFO 情報に表示するように設定されています。
NICK_SASET_HIDE_QUIT_ON
	現在 %s の最後に使った終了メッセージは %s INFO 情報に表示しないように設定されています。
NICK_SASET_HIDE_QUIT_OFF
	現在 %s の最後に使った終了メッセージは %s INFO 情報に表示するように設定されています。
NICK_SASET_HIDE_STATUS_ON
	現在 %s のサービスへアクセス状況は %s INFO 情報に表示しないように設定されています。
NICK_SASET_HIDE_STATUS_OFF
	現在 %s のサービスへアクセス状況は %s INFO 情報に表示するように設定されています。
	
# SASET MSG responses
NICK_SASET_MSG_SYNTAX
	SASET ニックネーム MSG {ON | OFF}
NICK_SASET_MSG_ON
	これから %s に対して各サービスは連絡手段としてメッセージを使います。
NICK_SASET_MSG_OFF
	これから %s に対して各サービスは連絡手段として告知を使います。

# SASET NOEXPIRE responses
NICK_SASET_NOEXPIRE_SYNTAX
	SASET ニックネーム NOEXPIRE {ON | OFF}
NICK_SASET_NOEXPIRE_ON
	%s というニックネームは期限切れになりません。
NICK_SASET_NOEXPIRE_OFF
	%s というニックネームは期限切れになります。

# SASET AUTOOP responses
NICK_SASET_AUTOOP_SYNTAX
	SASET ニックネーム AUTOOP {ON | OFF}
NICK_SASET_AUTOOP_ON
	サービスは %s に対して自動的に管理者権を与えるように設定しました。
NICK_SASET_AUTOOP_OFF
	サービスは %s に対して自動的に管理者権を与えないように設定しました。

# SASET LANGUAGE responses
NICK_SASET_LANGUAGE_SYNTAX
	SASET ニックネーム LANGUAGE 言語番号
NICK_SASET_LANGUAGE_UNKNOWN
	未知の言語番号 %d を指定しました。言語の一覧を見るには %R%s HELP SET LANGUAGE と入力してください。
NICK_SASET_LANGUAGE_CHANGED
	%s の使用言語を %s に切り替えました。


# ACCESS responses
NICK_ACCESS_SYNTAX
	ACCESS {ADD | DEL | LIST} [ホストマスク]
NICK_ACCESS_ALREADY_PRESENT
	%s というマスクは既に許可リストに登録されています。
NICK_ACCESS_REACHED_LIMIT
	1 つのニックネームにつき、%d 個以上の許可リストエントリを登録することは出来ません。
NICK_ACCESS_ADDED
	%s を許可リストに追加しました。
NICK_ACCESS_NOT_FOUND
	許可リストに %s は見つかりませんでした。
NICK_ACCESS_DELETED
	%s を許可リストから消去しました。
NICK_ACCESS_LIST
	許可リスト一覧: 
NICK_ACCESS_LIST_X
	%s の許可リスト一覧: 
NICK_ACCESS_LIST_EMPTY
	許可リストにはニックネームは登録されていません。
NICK_ACCESS_LIST_X_EMPTY
	%s の許可リストにはニックネームは登録されていません。

# Status messages
NICK_STATUS_0
	STATUS %s 0
NICK_STATUS_1
	STATUS %s 1
NICK_STATUS_2
	STATUS %s 2
NICK_STATUS_3
	STATUS %s 3

# INFO responses
NICK_INFO_SYNTAX
	INFO ニックネーム [ALL]
NICK_INFO_REALNAME
	%s の「本名」は %s です。
NICK_INFO_SERVICES_OPER
	%s はサービスオペレータです。
NICK_INFO_SERVICES_ADMIN
	%s は準サービス管理者です。
NICK_INFO_SERVICES_ROOT
	%s はサービス管理者です。
NICK_INFO_ADDRESS
	  最終接続アドレス: %s
NICK_INFO_ADDRESS_ONLINE
	            接続元: %s
NICK_INFO_ADDRESS_ONLINE_NOHOST
	%s は接続中です。
NICK_INFO_TIME_REGGED
	          登録日付: %s
NICK_INFO_LAST_SEEN
	      最終接続日時: %s
NICK_INFO_LAST_QUIT
	最終終了メッセージ: %s
NICK_INFO_URL
	               URL: %s
NICK_INFO_EMAIL
	電子メールアドレス: %s
NICK_INFO_VHOST
	  バーチャルホスト: %s
NICK_INFO_VHOST2
	  バーチャルホスト: %s@%s
NICK_INFO_ICQ
	          ICQ 番号: %d
NICK_INFO_GREET
	    歓迎メッセージ: %s
NICK_INFO_OPTIONS
	              設定: %s
NICK_INFO_EXPIRE
	            失効日: %s
# These strings MUST NOT be empty
NICK_INFO_OPT_KILL
	保護
NICK_INFO_OPT_SECURE
	セキュリティ
NICK_INFO_OPT_PRIVATE
	プライベート
NICK_INFO_OPT_MSG
	メッセージモード
NICK_INFO_OPT_AUTOOP
	自動管理者権割り当て
NICK_INFO_OPT_NONE
	無
NICK_INFO_NO_EXPIRE
	このニックネームに失効期限はありません。
NICK_INFO_FOR_MORE
	もっと詳しく知りたい場合は %R%s INFO %s ALL と入力してください。
NICK_INFO_SUSPENDED
	%s という理由により、このニックネームの使用を一時停止させていただいております。
NICK_INFO_SUSPENDED_NO_REASON
	このニックネームの使用を一時停止させていただいております。

# LIST responses
NICK_LIST_SYNTAX
	LIST パターン
NICK_LIST_SERVADMIN_SYNTAX
	LIST パターン [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
NICK_LIST_HEADER
	%s に一致するものの一覧:
NICK_LIST_RESULTS
	%d/%d の結果を表示しました。

# ALIST responses
NICK_ALIST_SYNTAX
	ALIST nickname
NICK_ALIST_HEADER
	あなたがアクセスしているチャンネル:
	 番号  チャンネル           レベル   備考
NICK_ALIST_HEADER_X
	%s がアクセスしているチャンネル:
	 番号  チャンネル           レベル   備考
NICK_ALIST_XOP_FORMAT
	  %3d %c%-20s %-8s %s
NICK_ALIST_ACCESS_FORMAT
	  %3d %c%-20s %-8d %s
NICK_ALIST_FOOTER
	リストはここまで - チャンネル数 %d/%d

# GLIST responses
NICK_GLIST_HEADER
	あなたと同じグループに所属しているニックネームのリスト:
NICK_GLIST_HEADER_X
	%s と同じグループに所属しているニックネームのリスト:
NICK_GLIST_FOOTER
	%d 個のニックネームがこのグループに登録されています。
NICK_GLIST_REPLY
	   %c%s
NICK_GLIST_REPLY_ADMIN
	   %c%s (%s 後に失効解除予定)

# RECOVER responses
NICK_RECOVER_SYNTAX
	RECOVER nickname [password]
NICK_NO_RECOVER_SELF
	自分自身を回復させることはできません!
NICK_RECOVERED
	あなたのニックネームを要求したユーザが kill されました。
	%R%s RELEASE %s のように入力して、%s がタイムアウトする前にニックネームを回復させてください。

# RELEASE responses
NICK_RELEASE_SYNTAX
	RELEASE nickname [password]
NICK_RELEASE_NOT_HELD
	ニックネーム %s はホールドされていません。
NICK_RELEASED
	サービスはニックネームを失効されないようにホールドします。

# GHOST responses
NICK_GHOST_SYNTAX
	GHOST nickname [password]
NICK_NO_GHOST_SELF
	自分自身はゴースト化できません!
NICK_GHOST_KILLED
	あなたのニックネームを使っていたゴーストが kill されました。

# GETPASS responses
NICK_GETPASS_SYNTAX
	GETPASS nickname
NICK_GETPASS_UNAVAILABLE
	暗号化が有効になっているため、GETPASS コマンドは使えません。
NICK_GETPASS_PASSWORD_IS
	%s のパスワードは %s です。

# GETEMAIL responses
NICK_GETEMAIL_SYNTAX
	GETEMAIL user@email-host No WildCards!!
NICK_GETEMAIL_EMAILS_ARE
	%s と %s に該当する電子メールアドレスは次の通りです。
NICK_GETEMAIL_NOT_USED
	%s に該当する電子メールアドレスは見つかりませんでした。
	
# SENDPASS responses
NICK_SENDPASS_SYNTAX
	SENDPASS nickname
NICK_SENDPASS_UNAVAILABLE
	暗号化が有効になっているため、SENDPASS コマンドは使えません。
NICK_SENDPASS_SUBJECT
	ニックネーム パスワード (%s)
NICK_SENDPASS_HEAD
	こんにちは、
NICK_SENDPASS_LINE_1
	ニックネーム %s のパスワードを電子メールで要求しました。
NICK_SENDPASS_LINE_2
	パスワードは %s です。セキュリティ確保のため、すぐにパスワードを変更してください。
NICK_SENDPASS_LINE_3
	このメールに身に覚えの無い場合は、無視してください。
NICK_SENDPASS_LINE_4
	このメールボックスはチェックされていません。このメールに対して返信しても返答はありません。
NICK_SENDPASS_LINE_5
	%s をご利用いただきありがとうございます。
NICK_SENDPASS_OK
	%s のパスワードが送信されました。

# SUSPEND responses
NICK_SUSPEND_SYNTAX
	SUSPEND nickname reason
NICK_SUSPEND_SUCCEEDED
	ニックネーム %s を一時的に使用禁止にしました。
NICK_SUSPEND_FAILED
	ニックネーム %s を一時的に使用禁止にできませんでした!

# UNSUSPEND responses
NICK_UNSUSPEND_SYNTAX
	UNSUSPEND nickname
NICK_UNSUSPEND_SUCCEEDED
	ニックネーム %s の登録を解除しました。
NICK_UNSUSPEND_FAILED
	ニックネーム %s の登録を解除できませんでした!

# FORBID responses
NICK_FORBID_SYNTAX
	FORBID nickname [reason]
NICK_FORBID_SYNTAX_REASON
	FORBID nickname reason
NICK_FORBID_SUCCEEDED
	現在、ニックネーム %s は使用禁止になっています。
NICK_FORBID_FAILED
	ニックネーム %s を使用禁止にできませんでした。

# Nick Registraion responses
NICK_REQUESTED
	このニックネームは現在、だれかが使用中です。あなたの電子メールアドレスとパスコードを確認してください。
NICK_REG_RESENT
	あなたのパスコードが %s 宛てに再送信されました。
NICK_REG_UNABLE
	ニックネームが登録されていません。時間をおいてやり直してください。
NICK_IS_PREREG
	ニックネーム登録を完了する前に電子メール確認コードの入力が必要です。
NICK_ENTER_REG_CODE
	%s 宛てにパスコードが送信されました。%R%s confirm <passcode> のように入力して、ニックネーム登録を完了させてください。
NICK_CONFIRM_NOT_FOUND
	ニックネーム登録手順 1 が期限切れになりました。"%R%s register <password> <email>" のように入力してください。
NICK_CONFIRM_INVALID
	パスコードが間違っています。電子メールアドレスを再確認し、やり直してください。
NICK_REG_MAIL_SUBJECT
	ニックネーム登録 (%s)
NICK_REG_MAIL_HEAD
	こんにちは、
NICK_REG_MAIL_LINE_1
	ニックネーム "%s" の登録申請ありがとうございます。
NICK_REG_MAIL_LINE_2
	登録を完了するには IRC クライアントで " %R%s confirm %s " のように入力してください。
NICK_REG_MAIL_LINE_3
	このメールに身に覚えの無い場合は、無視してください。
NICK_REG_MAIL_LINE_4
	このメールボックスはチェックされていません。このメールに対して返信しても返答はありません。
NICK_REG_MAIL_LINE_5
	%s をご利用いただきありがとうございます。
NICK_GETPASS_PASSCODE_IS
	%s のパスコードは %s です。
NICK_FORCE_REG
	ニックネーム %s の登録を完了しました。



###########################################################################
#
# ChanServ messages
#
###########################################################################

# Access level descriptions
CHAN_LEVEL_AUTOOP
	自動的にチャンネル運営者権を得る [AUTOOP] (+o)
CHAN_LEVEL_AUTOVOICE
	自動的にチャンネル発言権を得る [AUTOVOICE] (+v)
CHAN_LEVEL_AUTOHALFOP
	自動的にチャンネル準運営者権を得る [AUTOHALFOP] (+h)
CHAN_LEVEL_AUTOPROTECT
	自動的にニックネーム保護モードを得る [AUTOPROTECT] (+a)
CHAN_LEVEL_AUTODEOP
	チャンネルオペレータ状態を禁止される [AUTODEOP]
CHAN_LEVEL_NOJOIN
	チャンネルへの参加を禁止 [NOJOIN]
CHAN_LEVEL_INVITE
	INVITE (招待) コマンドの使用を許可
CHAN_LEVEL_AKICK
	AKICK (自動排除) コマンドの使用を許可
CHAN_LEVEL_SET
	SET (設定) コマンドの使用を許可 (FOUNDER/PASSWORD を除く) 
CHAN_LEVEL_CLEAR
	CLEAR (解除) コマンドの使用を許可
CHAN_LEVEL_UNBAN
	UNBAN (非排除化) コマンドの使用を許可
CHAN_LEVEL_OPDEOP
	OP/DEOP (チャンネル運営者権の付与/剥奪) コマンドの使用を許可
CHAN_LEVEL_ACCESS_LIST
	アクセスリストの閲覧を許可
CHAN_LEVEL_ACCESS_CHANGE
	アクセスリストの編集を許可
CHAN_LEVEL_MEMO
	チャンネルメモの list/read を許可
CHAN_LEVEL_ASSIGN
	ボットの assign/unassign (割り当て/解除) を許可
CHAN_LEVEL_BADWORDS
	BADWORDS (NG ワード) コマンドの使用を許可
CHAN_LEVEL_NOKICK
	ボットキッカーによってキックされない
CHAN_LEVEL_FANTASIA
	!op、!deop、!voice、!kick、!kb などのコマンド使用を許可
CHAN_LEVEL_SAY
	SAY 及び ACT コマンドの使用を許可
CHAN_LEVEL_GREET
	歓迎メッセージの表示
CHAN_LEVEL_VOICEME
	自身への VOICE/DEVOICE (発言権操作) コマンドの使用を許可
CHAN_LEVEL_VOICE
	VOICE/DEVOICE コマンドの使用を許可
CHAN_LEVEL_GETKEY
	GETKEY コマンドの使用を許可
CHAN_LEVEL_OPDEOPME
	自分自身にオペレータ権限を付与、剥奪することができます。
CHAN_LEVEL_HALFOPME
	自分自身に halfop 権限を付与、剥奪することができます。
CHAN_LEVEL_HALFOP
	HALFOP/DEHALFOP コマンドの使用を許可
CHAN_LEVEL_PROTECTME
	自分自身に protect 権限を付与、剥奪することができます。
CHAN_LEVEL_PROTECT
	PROTECT/DEPROTECT コマンドの使用を許可
CHAN_LEVEL_KICKME
	自分自身をキックできます。
CHAN_LEVEL_KICK
	KICK コマンドの使用を許可
CHAN_LEVEL_SIGNKICK
	SIGNKICK レベルが有効な場合、キックできません。
CHAN_LEVEL_BANME
	自分自身をバンできます。
CHAN_LEVEL_BAN
	BAN コマンドの使用を許可
CHAN_LEVEL_TOPIC
	TOPIC コマンドの使用を許可
CHAN_LEVEL_INFO
	ALL オプション付きで INFO コマンドを使えます。

# Automatic responses
CHAN_IS_REGISTERED
	このチャンネルは既に %s により登録されています。
CHAN_NOT_ALLOWED_OP
	チャンネル %s ではチャンネルオペレータになれません。
CHAN_MAY_NOT_BE_USED
	このチャンネルは使用されていません。
CHAN_NOT_ALLOWED_TO_JOIN
	このチャンネルへの参加は許可されていません。
CHAN_X_INVALID
	チャンネル %s は無効なチャンネルです。

# REGISTER responses
CHAN_REGISTER_SYNTAX
	REGISTER channel password description
CHAN_REGISTER_DISABLED
	ご迷惑をかけて申し訳ありません。チャンネル登録は一時的に禁止されています。
CHAN_REGISTER_NOT_LOCAL
	ローカルチャンネルは登録できません。
CHAN_MUST_REGISTER_NICK
	先にニックネームの登録を行ってください。%R%s HELP と入力することでより詳細な情報が見られます。
CHAN_MUST_IDENTIFY_NICK
	先に以下のコマンドで %s の認証を行ってください:
	%R%s IDENTIFY password
CHAN_MAY_NOT_BE_REGISTERED
	チャンネル %s は未登録です。
CHAN_ALREADY_REGISTERED
	チャンネル %s は登録済みです!
CHAN_MUST_BE_CHANOP
	チャンネルを登録するにはオペレータ権限が必要です。
CHAN_REACHED_CHANNEL_LIMIT
	ご迷惑をおかけして申し訳ありません。参加中のチャンネル数が %d に達したため、これ以上参加できません。
CHAN_EXCEEDED_CHANNEL_LIMIT
	ご迷惑をおかけして申し訳ありません。参加中のチャンネル数が %d 以上のため、これ以上参加できません。
CHAN_REGISTRATION_FAILED
	ご迷惑をおかけして申し訳ありません。登録に失敗しました。
CHAN_REGISTERED
	チャンネル %s はあなたのニックネーム %s で登録済みです。
CHAN_PASSWORD_IS
	チャンネルパスワードは %s です。参加するにはこれが必要になります。
CHAN_REGISTER_NONE_CHANNEL
	存在しないチャンネル %s は登録できません。
CHAN_SYMBOL_REQUIRED
	登録の際には、# を使用してください。


# IDENTIFY responses
CHAN_IDENTIFY_SYNTAX
	IDENTIFY チャンネル パスワード
CHAN_IDENTIFY_FAILED
	ご迷惑をおかけして申し訳ありません。認証に失敗しました。
CHAN_IDENTIFY_SUCCEEDED
	認証に成功しました。設立者権限で %s へのアクセスが可能です。

# LOGOUT responses
CHAN_LOGOUT_SYNTAX
	LOGOUT channel nickname
CHAN_LOGOUT_SERVADMIN_SYNTAX
	LOGOUT channel [nickname]
CHAN_LOGOUT_SUCCEEDED
	 ユーザ %s がチャンネル %s からログアウトしました。
CHAN_LOGOUT_ALL_SUCCEEDED
	チャンネル設立者を除く指定されたユーザ全員をチャンネル %s からログアウトさせました。
CHAN_LOGOUT_FOUNDER_FAILED
	チャンネル設立者は %s からログアウトできません。

# DROP responses
CHAN_DROP_SYNTAX
	DROP channel
CHAN_DROP_DISABLED
	ご迷惑を掛けて申し訳ございません。一時的にチャンネルの登録を解除することができなくなっています。
CHAN_DROPPED
	チャンネル %s の登録が解除されました。

# SET responses
CHAN_SET_SYNTAX
	SET channel option parameters
CHAN_SET_DISABLED
	ご迷惑を掛けて申し訳ございません。一時的にチャンネルオプションを変更することができなくなっています。
CHAN_SET_UNKNOWN_OPTION
	未定義の SET オプション: %s。

# SET FOUNDER responses
CHAN_SET_FOUNDER_TOO_MANY_CHANS
	%s が登録できるチャンネル数の上限に到達しました。
CHAN_FOUNDER_CHANGED
	チャンネル %s の登録メンバーが %s に変更されました。

# SET SUCCESSOR responses
CHAN_SUCCESSOR_CHANGED
	チャンネル %s のメンテナ権を %s に譲渡しました。
CHAN_SUCCESSOR_UNSET
	チャンネル %s のメンテナが解除されました。
CHAN_SUCCESSOR_IS_FOUNDER
	%s はチャンネル設立者であるためチャンネル %s のメンテナになれません。

# SET PASSWORD responses
CHAN_SET_PASSWORD_FAILED
	パスワードを設定できません。
CHAN_PASSWORD_CHANGED
	%s のパスワードを変更しました。
CHAN_PASSWORD_CHANGED_TO
	%s のパスワードを %s に変更しました。

# SET DESC responses
CHAN_DESC_CHANGED
	チャンネル %s の説明を %s に変更しました。

# SET URL responses
CHAN_URL_CHANGED
	チャンネル %s の URL を %s に変更しました。
CHAN_URL_UNSET
	チャンネル %s の URL を未設定に戻しました。

# SET EMAIL responses
CHAN_EMAIL_CHANGED
	チャンネル %s の電子メールアドレスを %s に変更しました。
CHAN_EMAIL_UNSET
	チャンネル %s の電子メールアドレスを未設定に戻しました。

# SET ENTRYMSG responses
CHAN_ENTRY_MSG_CHANGED
	チャンネル %s の参加メッセージを変更しました。
CHAN_ENTRY_MSG_UNSET
	チャンネル %s の参加メッセージを未設定に戻しました。
	
# SET BANTYPE responses
CHAN_SET_BANTYPE_INVALID
	%s は有効な禁止タイプではありません。
CHAN_SET_BANTYPE_CHANGED
	チャンネル %s の現在の禁止タイプは #%d です。

# SET MLOCK responses
CHAN_SET_MLOCK_UNKNOWN_CHAR
	認識できないモードキャラクタ %c を無視しました。
CHAN_SET_MLOCK_IMPOSSIBLE_CHAR
	ロック権限が無いのでモード %c を無視しました。
CHAN_SET_MLOCK_L_REQUIRED
	ロックモード +L を ON にするのと同時に +l も ON にしてください。
CHAN_SET_MLOCK_K_REQUIRED
	ロックモード +K を ON にするのと同時に +i も ON にしてください。
CHAN_MLOCK_CHANGED
	チャンネル %s のロックモードを %s に変更しました。

# SET KEEPTOPIC responses
CHAN_SET_KEEPTOPIC_SYNTAX
	SET チャンネル名 KEEPTOPIC {ON | OFF}
CHAN_SET_KEEPTOPIC_ON
	チャンネル %s のトピック保存オプションは ON です。
CHAN_SET_KEEPTOPIC_OFF
	チャンネル %s のトピック保存オプションは OFF です。

# SET TOPICLOCK responses
CHAN_SET_TOPICLOCK_SYNTAX
	SET チャンネル名 TOPICLOCK {ON | OFF}
CHAN_SET_TOPICLOCK_ON
	チャンネル %s のトピックロックオプションは ON です。
CHAN_SET_TOPICLOCK_OFF
	チャンネル %s のトピックロックオプションは OFF です。

# SET PEACE responses
CHAN_SET_PEACE_SYNTAX
	SET チャンネル名 PEACE {ON | OFF}
CHAN_SET_PEACE_ON
	チャンネル %s の Peace オプションは ON です。
CHAN_SET_PEACE_OFF
	チャンネル %s の Peace オプションは OFF です。

# SET PRIVATE responses
CHAN_SET_PRIVATE_SYNTAX
	SET チャンネル名 PRIVATE {ON | OFF}
CHAN_SET_PRIVATE_ON
	チャンネル %s のプライベートオプションは ON です。
CHAN_SET_PRIVATE_OFF
	チャンネル %s のプライベートオプションは OFF です。

# SET SECUREOPS responses
CHAN_SET_SECUREOPS_SYNTAX
	SET チャンネル名 SECUREOPS {ON | OFF}
CHAN_SET_SECUREOPS_ON
	チャンネル %s のセキュアオペレータオプションは ON です。
CHAN_SET_SECUREOPS_OFF
	チャンネル %s のセキュアオペレータオプションは OFF です。

# SET SECUREFOUNDER responses
CHAN_SET_SECUREFOUNDER_SYNTAX
	SET チャンネル名 SECUREFOUNDER {ON | OFF}
CHAN_SET_SECUREFOUNDER_ON
	チャンネル %s のセキュア設立者オプションは ON です。
CHAN_SET_SECUREFOUNDER_OFF
	チャンネル %s のセキュア設立者オプションは OFF です。

# SET RESTRICTED responses
CHAN_SET_RESTRICTED_SYNTAX
	SET チャンネル名 RESTRICTED {ON | OFF}
CHAN_SET_RESTRICTED_ON
	チャンネル %s の制限アクセスオプションは ON です。
CHAN_SET_RESTRICTED_OFF
	チャンネル %s の制限アクセスオプションは OFF です。

# SET SECURE responses
CHAN_SET_SECURE_SYNTAX
	SET チャンネル名 SECURE {ON | OFF}
CHAN_SET_SECURE_ON
	チャンネル %s のセキュアオプションは ON です。
CHAN_SET_SECURE_OFF
	チャンネル %s のセキュアオプションは OFF です。

# SET SIGNKICK responses
CHAN_SET_SIGNKICK_SYNTAX
	SET チャンネル名 SIGNKICK {ON | LEVEL | OFF}
CHAN_SET_SIGNKICK_ON
	チャンネル %s の Signed kick オプションは ON です。
CHAN_SET_SIGNKICK_LEVEL
	チャンネル %s の Signed kick オプションは ON ですが、
	レベルはコマンドを発行したユーザの設定に依存します。
CHAN_SET_SIGNKICK_OFF
	チャンネル %s の Signed kick オプションは OFF です。

# SET OPNOTICE responses
CHAN_SET_OPNOTICE_SYNTAX
	SET チャンネル名 OPNOTICE {ON | OFF}
CHAN_SET_OPNOTICE_ON
	チャンネル %s の Op-notice オプションは ON です。
CHAN_SET_OPNOTICE_OFF
	チャンネル %s の Op-notice オプションは OFF です。

# SET XOP responses
CHAN_SET_XOP_SYNTAX
	SET チャンネル名 XOP {ON | OFF}
CHAN_SET_XOP_ON
	チャンネル %s の xOP リストシステムは ON です。
CHAN_SET_XOP_OFF
	チャンネル %s の xOP リストシステムは OFF です。

# SET NOEXPIRE responses
CHAN_SET_NOEXPIRE_SYNTAX
	SET チャンネル名 NOEXPIRE {ON | OFF}
CHAN_SET_NOEXPIRE_ON
	チャンネル %s はユーザがいなくなってもなくなりません。
CHAN_SET_NOEXPIRE_OFF
	チャンネル %s はユーザがいなくなったらなくなります。

# xOP messages
CHAN_XOP_REACHED_LIMIT
	AOP/SOP/VOP エントリを %d 個より多く登録することは出来ません。
CHAN_XOP_LIST_FORMAT
	  %3d  %s
CHAN_XOP_ACCESS
	このコマンドは使用できません。 ACCESS コマンドを使用してください。
	より詳しい情報は %R%s HELP ACCESS と入力してください。
CHAN_XOP_NOT_AVAILABLE
	xOP システムは使用できません。

# AOP messages
CHAN_AOP_SYNTAX
	AOP チャンネル名 {ADD|DEL|LIST|CLEAR} [ニックネーム | エントリ番号]
CHAN_AOP_DISABLED
	チャンネルの AOP リストの変更は一時的に禁止されています。
CHAN_AOP_NICKS_ONLY
	チャンネルの AOP リストに追加できるニックネームは登録されたものでなければいけません。
CHAN_AOP_ADDED
	%s をチャンネル %s の AOP リストに追加しました。
CHAN_AOP_MOVED
	%s をチャンネル %s の AOP リストに移動しました。
CHAN_AOP_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s の AOP リストにありません。
CHAN_AOP_NOT_FOUND
	%s はチャンネル %s の AOP リストにありません。
CHAN_AOP_NO_MATCH
	チャンネル %s の AOP リストにマッチするエントリがありません。
CHAN_AOP_DELETED
	%s をチャンネル %s の AOP リストから削除しました。
CHAN_AOP_DELETED_ONE
	1 個のエントリをチャンネル %s の AOP リストから削除しました。
CHAN_AOP_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s の AOP リストから削除しました。
CHAN_AOP_LIST_EMPTY
	チャンネル %s の AOP リストは空です。
CHAN_AOP_LIST_HEADER
	チャンネル %s の AOP リスト:
	  番号 ニックネーム
CHAN_AOP_CLEAR
	チャンネル %s の AOP リストを消去しました。

# HOP messages
CHAN_HOP_SYNTAX
	HOP チャンネル名 {ADD|DEL|LIST|CLEAR} [ニックネーム | エントリ番号]
CHAN_HOP_DISABLED
	チャンネルの HOP リストの変更は一時的に禁止されています。
CHAN_HOP_NICKS_ONLY
	チャンネルの HOP リストに追加できるニックネームは登録されたものでなければいけません。
CHAN_HOP_ADDED
	%s をチャンネル %s の HOP リストに追加しました。
CHAN_HOP_MOVED
	%s をチャンネル %s の HOP リストに移動しました。
CHAN_HOP_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s の HOP リストにありません。
CHAN_HOP_NOT_FOUND
	%s はチャンネル %s の HOP リストにありません。
CHAN_HOP_NO_MATCH
	チャンネル %s の HOP リストにマッチするエントリがありません。
CHAN_HOP_DELETED
	%s をチャンネル %s の HOP リストから削除しました。
CHAN_HOP_DELETED_ONE
	1 個のエントリをチャンネル %s の HOP リストから削除しました。
CHAN_HOP_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s の HOP リストから削除しました。
CHAN_HOP_LIST_EMPTY
	チャンネル %s の HOP リストは空です。
CHAN_HOP_LIST_HEADER
	チャンネル %s の HOP リスト:
	  番号 ニックネーム
CHAN_HOP_CLEAR
	チャンネル %s の HOP リストを消去しました。

# SOP messages
CHAN_SOP_SYNTAX
	SOP チャンネル名 {ADD|DEL|LIST|CLEAR} [ニックネーム | エントリ番号]
CHAN_SOP_DISABLED
	チャンネルの SOP リストの変更は一時的に禁止されています。
CHAN_SOP_NICKS_ONLY
	チャンネルの SOP リストに追加できるニックネームは登録されたものでなければいけません。
CHAN_SOP_ADDED
	%s をチャンネル %s の SOP リストに追加しました。
CHAN_SOP_MOVED
	%s をチャンネル %s の SOP リストに移動しました。
CHAN_SOP_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s の SOP リストにありません。
CHAN_SOP_NOT_FOUND
	%s はチャンネル %s の SOP リストにありません。
CHAN_SOP_NO_MATCH
	チャンネル %s の SOP リストにマッチするエントリがありません。
CHAN_SOP_DELETED
	%s をチャンネル %s の SOP リストから削除しました。
CHAN_SOP_DELETED_ONE
	1 個のエントリをチャンネル %s の SOP リストから削除しました。
CHAN_SOP_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s の SOP リストから削除しました。
CHAN_SOP_LIST_EMPTY
	チャンネル %s の SOP リストは空です。
CHAN_SOP_LIST_HEADER
	チャンネル %s の SOP リスト:
	  番号 ニックネーム
CHAN_SOP_CLEAR
	チャンネル %s の SOP リストを消去しました。

# VOP messages
CHAN_VOP_SYNTAX
	VOP チャンネル名 {ADD|DEL|LIST|CLEAR} [ニックネーム | エントリ番号]
CHAN_VOP_DISABLED
	チャンネルの VOP リストの変更は一時的に禁止されています。
CHAN_VOP_NICKS_ONLY
	チャンネルの VOP リストに追加できるニックネームは登録されたものでなければいけません。
CHAN_VOP_ADDED
	%s をチャンネル %s の VOP リストに追加しました。
CHAN_VOP_MOVED
	%s をチャンネル %s の VOP リストに移動しました。
CHAN_VOP_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s の VOP リストにありません。
CHAN_VOP_NOT_FOUND
	%s はチャンネル %s の VOP リストにありません。
CHAN_VOP_NO_MATCH
	チャンネル %s の VOP リストにマッチするエントリがありません。
CHAN_VOP_DELETED
	%s をチャンネル %s の VOP リストから削除しました。
CHAN_VOP_DELETED_ONE
	1 個のエントリをチャンネル %s の VOP リストから削除しました。
CHAN_VOP_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s の VOP リストから削除しました。
CHAN_VOP_LIST_EMPTY
	チャンネル %s の VOP リストは空です。
CHAN_VOP_LIST_HEADER
	チャンネル %s の VOP リスト:
	  番号 ニックネーム
CHAN_VOP_CLEAR
	チャンネル %s の VOP リストを消去しました。

# ACCESS messages
CHAN_ACCESS_SYNTAX
	ACCESS チャンネル名 {ADD|DEL|LIST|CLEAR} [ニックネーム [レベル] | エントリ番号]
CHAN_ACCESS_XOP
	このコマンドを使うことは出来ません。
	AOP、SOP 又は VOP コマンドを使ってください。
	より詳しい情報は %R%s HELP command と入力してください。
CHAN_ACCESS_XOP_HOP
	このコマンドを使うことは出来ません。
	AOP、SOP、HOP 又は VOP コマンドを使ってください。
	より詳しい情報は %R%s HELP command と入力してください。
CHAN_ACCESS_DISABLED
	チャンネルのアクセスリストの変更は一時的に禁止されています。
CHAN_ACCESS_LEVEL_NONZERO
	アクセスレベルはゼロ以外の値を指定してください。
CHAN_ACCESS_LEVEL_RANGE
	アクセスレベルは %d から %d の間の値を指定してください。
CHAN_ACCESS_NICKS_ONLY
	チャンネルのアクセスリストに追加できるニックネームは登録されたものでなければいけません。
CHAN_ACCESS_REACHED_LIMIT
	チャンネルのアクセスエントリを %d 個以上指定することは出来ません。
CHAN_ACCESS_LEVEL_UNCHANGED
	%s のアクセスレベル (チャンネル %s) を %d から変更しませんでした。
CHAN_ACCESS_LEVEL_CHANGED
	%s のアクセスレベル (チャンネル %s) を %d に変更しました。
CHAN_ACCESS_ADDED
	%s をチャンネル %s のアクセスリスト、レベル %d に追加しました。
CHAN_ACCESS_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s のアクセスリストにありません。
CHAN_ACCESS_NOT_FOUND
	%s はチャンネル %s のアクセスリストにありません。
CHAN_ACCESS_NO_MATCH
	チャンネル %s のアクセスリストにマッチするエントリがありません。
CHAN_ACCESS_DELETED
	 %s をチャンネル %s のアクセスリストから削除しました。
CHAN_ACCESS_DELETED_ONE
	1 個のエントリをチャンネル %s のアクセスリストから削除しました。
CHAN_ACCESS_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s のアクセスリストから削除しました。
CHAN_ACCESS_LIST_EMPTY
	チャンネル %s のアクセスリストは空です。
CHAN_ACCESS_LIST_HEADER
	チャンネル %s のアクセスリスト:
	  番号  レベル ニックネーム
CHAN_ACCESS_LIST_FOOTER
	アクセスリストの最後。
CHAN_ACCESS_LIST_XOP_FORMAT
	  %3d   %s  %s
CHAN_ACCESS_LIST_AXS_FORMAT
	  %3d  %4d  %s
CHAN_ACCESS_CLEAR
	チャンネル %s の AOP リストを消去しました。

# AKICK responses
CHAN_AKICK_SYNTAX
	AKICK チャンネル {ADD | STICK | UNSTICK | DEL | LIST | VIEW | ENFORCE | CLEAR} [ニックネーム又はマスク] [理由]
CHAN_AKICK_DISABLED
	チャンネルの自動キックリストの変更は一時的に禁止されています。
CHAN_AKICK_ALREADY_EXISTS
	%s はチャンネル %s の自動キックリストに登録済みです。
CHAN_AKICK_REACHED_LIMIT
	%d 個以上の自動キックマスクをつけることは出来ません。
CHAN_AKICK_ADDED
	%s をチャンネル %s の自動キックリストに追加しました。
CHAN_AKICK_NO_SUCH_ENTRY
	エントリ (#%d) はチャンネル %s の自動キックリストにありません。
CHAN_AKICK_NOT_FOUND
	%s はチャンネル %s の自動キックリストにありません。
CHAN_AKICK_NO_MATCH
	チャンネル %s の自動キックリストにマッチするエントリがありません。
CHAN_AKICK_STUCK
	%s に対するキックはチャンネル %s で常に有効です。
CHAN_AKICK_UNSTUCK
	%s に対するキックはチャンネル %s で常に無効です。
CHAN_AKICK_DELETED
	%s をチャンネル %s の自動キックリストから削除しました。
CHAN_AKICK_DELETED_ONE
	1 個のエントリをチャンネル %s の自動キックリストから削除しました。
CHAN_AKICK_DELETED_SEVERAL
	%d 個のエントリをチャンネル %s の自動キックリストから削除しました。
CHAN_AKICK_LIST_EMPTY
	チャンネル %s の自動キックリストは空です。
CHAN_AKICK_LIST_HEADER
	チャンネル %s の自動キックリスト:
CHAN_AKICK_LIST_FORMAT
	  %3d %s (%s)
CHAN_AKICK_VIEW_FORMAT
	%3d %s (設定者 %s 設定日時 %s)
	    %s
CHAN_AKICK_VIEW_FORMAT_STUCK
	%3d %s (stuck) (設定者 %s 設定日時 %s)
	    %s
CHAN_AKICK_ENFORCE_DONE
	%s に対する AKICK ENFORCE コマンド完了; %d 人のユーザに影響が出ました。
CHAN_AKICK_CLEAR
	チャンネル %s の自動キックリストを消去しました。

# LEVELS responses
CHAN_LEVELS_SYNTAX
	LEVELS チャンネル {SET | DIS[ABLE] | LIST | RESET} [アイテム [レベル]]
CHAN_LEVELS_XOP
	このチャンネルでは xOP が有効なので、レベル設定を使えません。
CHAN_LEVELS_RANGE
	レベルは %d から %d の間に設定してください。
CHAN_LEVELS_CHANGED
	%s に対するレベル (チャンネル %s) を %d に変更しました。
CHAN_LEVELS_UNKNOWN
	設定 %s は定義されていません。定義された設定のリストは %R%s HELP LEVELS DESC と入力してください。
CHAN_LEVELS_DISABLED
	アクセスレベル %s (チャンネル %s) の使用を禁止しました。
CHAN_LEVELS_LIST_HEADER
	チャンネル %s のアクセスレベル:
CHAN_LEVELS_LIST_DISABLED
	    %-*s  (使用禁止)
CHAN_LEVELS_LIST_FOUNDER
	    %-*s  (チャンネル設立者のみ)
CHAN_LEVELS_LIST_NORMAL
	    %-*s  %d
CHAN_LEVELS_RESET
	%s のアクセスレベルを初期値に戻しました。

# Status Messages
CHAN_STATUS_SYNTAX
	STATUS ERROR 構文エラー
CHAN_STATUS_NOT_REGGED
	STATUS ERROR チャンネル %s は登録されていません。
CHAN_STATUS_FORBIDDEN
	STATUS ERROR チャンネル %s は禁止されています。
CHAN_STATUS_NOTONLINE
	STATUS ERROR ニックネーム %s はオフラインです。
CHAN_STATUS_INFO
	STATUS %s %s %d

# INFO responses
CHAN_INFO_SYNTAX
	INFO channel [ALL]
CHAN_INFO_HEADER
	チャンネル %s の情報:
CHAN_INFO_FOUNDER
	         設立者: %s (%s)
CHAN_INFO_NO_FOUNDER
	         設立者: %s
CHAN_INFO_SUCCESSOR
	         後継者: %s (%s)
CHAN_INFO_NO_SUCCESSOR
	         後継者: %s
CHAN_INFO_DESCRIPTION
	           説明: %s
CHAN_INFO_ENTRYMSG
	 参加メッセージ: %s
CHAN_INFO_TIME_REGGED
	       登録済み: %s
CHAN_INFO_LAST_USED
	     最新の使用: %s
CHAN_INFO_LAST_TOPIC
	 最新のトピック: %s
CHAN_INFO_TOPIC_SET_BY
	トピックを設定したユーザ: %s
CHAN_INFO_URL
	            URL: %s
CHAN_INFO_EMAIL
	電子メールアドレス: %s
CHAN_INFO_BANTYPE
	     禁止タイプ: %d
CHAN_INFO_OPTIONS
	     オプション: %s
CHAN_INFO_OPT_KEEPTOPIC
	トピック保存
CHAN_INFO_OPT_OPNOTICE
	OP Notice
CHAN_INFO_OPT_PEACE
	Peace
CHAN_INFO_OPT_PRIVATE
	プライベート
CHAN_INFO_OPT_RESTRICTED
	アクセス制限
CHAN_INFO_OPT_SECURE
	セキュア
CHAN_INFO_OPT_SECUREOPS
	セキュアオペレータ
CHAN_INFO_OPT_SECUREFOUNDER
	セキュア設立者
CHAN_INFO_OPT_SIGNKICK
	Signed kicks
CHAN_INFO_OPT_TOPICLOCK
	トピックロック
CHAN_INFO_OPT_XOP
	xOP リストシステム
CHAN_INFO_OPT_NONE
	無
CHAN_INFO_MODE_LOCK
	   モードロック: %s
CHAN_INFO_EXPIRE
	        失効日時: %s
CHAN_INFO_NO_EXPIRE
	このチャンネルはユーザがいなくなってもなくなりません。

# LIST responses
CHAN_LIST_SYNTAX
	LIST pattern
CHAN_LIST_SERVADMIN_SYNTAX
	LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
CHAN_LIST_HEADER
	%s にマッチするリスト:
CHAN_LIST_FORMAT
	    %-20s  %s
CHAN_LIST_END
	リストの最後 - %d/%d 個のマッチしたエントリ。
	
# INVITE responses
CHAN_INVITE_SYNTAX
	INVITE channel

# UNBAN responses
CHAN_UNBAN_SYNTAX
	UNBAN channel
CHAN_UNBANNED
	%s から禁止解除さしました。

# TOPIC responses
CHAN_TOPIC_SYNTAX
	TOPIC channel [topic]

# CLEAR responses
CHAN_CLEAR_SYNTAX
	CLEAR channel what
CHAN_CLEARED_BANS
	チャンネル %s の禁止リストを初期化しました。
CHAN_CLEARED_EXCEPTS
	チャンネル %s の除外リストを初期化しました。
CHAN_CLEARED_MODES
	チャンネル %s のモードからすべてのモードを初期化しました。
CHAN_CLEARED_OPS
	チャンネル %s のモードから +o を無効化しました。
CHAN_CLEARED_HOPS
	チャンネル %s のモードから +h を無効化しました。
CHAN_CLEARED_VOICES
	チャンネル %s のモードから +v を無効化しました。
CHAN_CLEARED_USERS
	チャンネル %s からすべてのユーザをキックしました。
CHAN_CLEARED_INVITES
	チャンネル %s の招聘リストを初期化しました。

# GETPASS responses
CHAN_GETPASS_SYNTAX
	GETPASS channel
CHAN_GETPASS_UNAVAILABLE
	暗号化されているため、GETPASS コマンドは使えません。
CHAN_GETPASS_PASSWORD_IS
	チャンネル %s のパスワードは %s です。

# GETKEY responses
CHAN_GETKEY_SYNTAX
	GETKEY channel
CHAN_GETKEY_NOKEY
	チャンネル %s にキーが設定されていません。
CHAN_GETKEY_KEY
	KEY %s %s

# SENDPASS responses
CHAN_SENDPASS_SYNTAX
	SENDPASS channel
CHAN_SENDPASS_UNAVAILABLE
	暗号化されているため、SENDPASS コマンドは使えません。
CHAN_SENDPASS_SUBJECT
	チャンネルパスワード (%s)
CHAN_SENDPASS_HEAD
	こんにちわ。
CHAN_SENDPASS_LINE_1
	チャンネル %s のパスワードを電子メールで要求しました。
CHAN_SENDPASS_LINE_2
	パスワードは %s です。セキュリティ確保のため、すぐにパスワードを変更してください。
CHAN_SENDPASS_LINE_3
	このメールに身に覚えの無い場合は、無視してください。
CHAN_SENDPASS_LINE_4
	このメールボックスはチェックされていません。このメールに対して返信しても返答はありません。
CHAN_SENDPASS_LINE_5
	%s をご利用いただきありがとうございます。
CHAN_SENDPASS_OK
	チャンネル %s のパスワードが送信されました。

# FORBID responses
CHAN_FORBID_SYNTAX
	FORBID channel [reason]
CHAN_FORBID_SYNTAX_REASON
	FORBID channel reason
CHAN_FORBID_SUCCEEDED
	チャンネル %s を禁止にしました。
CHAN_FORBID_FAILED
	チャンネル %s を禁止に出来ません!
CHAN_FORBID_REASON
	このチャンネルは禁止にされています。

# SUSPEND responses
CHAN_SUSPEND_SYNTAX
	SUSPEND channel [reason]
CHAN_SUSPEND_SYNTAX_REASON
	SUSPEND channel reason
CHAN_SUSPEND_SUCCEEDED
	チャンネル %s を休止状態にしました。
CHAN_SUSPEND_FAILED
	チャンネル %s を休止状態に出来ません!
CHAN_SUSPEND_REASON
	このチャンネルは休止状態です。

# UNSUSPEND responses
CHAN_UNSUSPEND_SYNTAX
	UNSUSPEND channel
CHAN_UNSUSPEND_ERROR
	# がチャンネル名の前にありません。
CHAN_UNSUSPEND_SUCCEEDED
	チャンネル %s をリリースしました。
CHAN_UNSUSPEND_FAILED
	チャンネル %s をリリースできません!

# Misc responses
CHAN_EXCEPTED
	%s は %s を除いてマッチし、例外が解除されるまではバンできません。

###########################################################################
#
# MemoServ messages
#
###########################################################################

# Automatic messages
MEMO_HAVE_NEW_MEMO
	1 通の新しいメモがあります。
MEMO_HAVE_NEW_MEMOS
	%d 通の新しいメモがあります。
MEMO_TYPE_READ_LAST
	%R%s READ LAST で最新のメモを読むことができます。
MEMO_TYPE_READ_NUM
	%R%s READ %d でメモを読むことができます。
MEMO_TYPE_LIST_NEW
	%R%s LIST NEW で新着メモのリストが表示されます。
MEMO_AT_LIMIT
	警告: あなたが受け取れるメモの最大数 (%d) に達しました。現在のメモを削除しない限り、これ以上の新着メモを受け取ることはできません。
MEMO_OVER_LIMIT
	警告: 既にあなたが受け取れるメモの最大数 (%d) に達しています。今あるメモを削除しない限り、これ以上の新着メモを受け取ることはできません。
MEMO_X_MANY_NOTICE
	%d 通のメモがチャンネル %s にあります。
MEMO_X_ONE_NOTICE
	%d 通のメモがチャンネル %s にあります。
MEMO_NEW_X_MEMO_ARRIVED
	チャンネル %s に新着メモがあります。
	メモを読むには、%R%s READ %s %d と入力してください。
MEMO_NEW_MEMO_ARRIVED
	%s からあなた宛に新着メモを受け取りました。
	メモを読むには、%R%s READ %d と入力してください。

# Multi-use responses
MEMO_HAVE_NO_MEMOS
	メモを受け取っていません。
MEMO_X_HAS_NO_MEMOS
	%s はメモを受け取っていません。
MEMO_DOES_NOT_EXIST
	%d 番目ののメモが見つかりませんでした!
MEMO_LIST_NOT_FOUND
	条件にマッチするメモが見つかりませんでした。

# SEND responses
MEMO_SEND_SYNTAX
	SEND {nick | channel} memo-text
MEMO_SEND_DISABLED
	ご迷惑をおかけして申し訳ございません。一時的にメモを送ることができない状態です。
MEMO_SEND_PLEASE_WAIT
	%d 秒待ってから再度 SEND コマンドをやり直してください。
MEMO_X_GETS_NO_MEMOS
	%s はメモを受け取ることができません。
MEMO_X_HAS_TOO_MANY_MEMOS
	%s はこれ以上メモを受け取ることができません。
MEMO_SENT
	%s 宛てにメモを送信しました。
MEMO_MASS_SENT
	登録済みのユーザ全員宛にメモの大量送信が行われました。
	
# STAFF responses
MEMO_STAFF_SYNTAX
	STAFF memo-text
	
# CANCEL responses
MEMO_CANCEL_SYNTAX
	CANCEL {nick | channel}
MEMO_CANCEL_DISABLED
	ご迷惑をおかけして申し訳ありません。一時的にメモの送信をキャンセルすることができない状態になっています。
MEMO_CANCEL_NONE
	メモの送信がキャンセルされませんでした。
MEMO_CANCELLED
	最新の %s 宛てのメモ送信がキャンセルされました。

# LIST responses
MEMO_LIST_SYNTAX
	LIST [channel] [list | NEW]
MEMO_HAVE_NO_NEW_MEMOS
	新着メモがありません。
MEMO_X_HAS_NO_NEW_MEMOS
	%s 宛ての新着メモはありません。
MEMO_LIST_MEMOS
	%s 宛てのメモがあります。読むには、%R%s READ num のように入力してください。
MEMO_LIST_NEW_MEMOS
	%s 宛ての新着メモがあります。読むには、%R%s READ num のように入力してください。
MEMO_LIST_CHAN_MEMOS
	%s 宛てのメモがあります。読むには、%R%s READ %s num のように入力してください。
MEMO_LIST_CHAN_NEW_MEMOS
	%s 宛ての新着メモがあります。読むには、%R%s READ %s num のように入力してください。
MEMO_LIST_HEADER
	 番号 送信者            日付/時間
MEMO_LIST_FORMAT
	%c%3d  %-16s  %s

# READ responses
MEMO_READ_SYNTAX
	READ [channel] {list | LAST | NEW}
MEMO_HEADER
	%d 番目のメモ (送信者: %s 内容: %s) を削除するには、%R%s DEL %d のように入力してください。
MEMO_CHAN_HEADER
	%d 番目のメモ (送信者: %s 内容: %s) を削除するには、%R%s DEL %s %d のように入力してください。
MEMO_TEXT
	%s

# DEL responses
MEMO_DEL_SYNTAX
	DEL [channel] {num | list | ALL}
MEMO_DELETED_NONE
	これ以上メモを削除することはできません。
MEMO_DELETED_ONE
	%d 番目のメモを削除しました。
MEMO_DELETED_SEVERAL
	%s のメモを削除しました。
MEMO_DELETED_ALL
	あなた宛てのメモを全て削除しました。
MEMO_CHAN_DELETED_ALL
	チャンネル %s 宛てのすべてのメモを削除しました。

# SET responses
MEMO_SET_SYNTAX
	SET option parameters
MEMO_SET_DISABLED
	ご迷惑をかけて申し訳ありません。メモ機能に対するオプション操作は一時的に使うことができない状態になっています。
MEMO_SET_UNKNOWN_OPTION
	メモ機能の SET オプションで %s という引数を使うことはできません。

# SET NOTIFY responses
MEMO_SET_NOTIFY_SYNTAX
	SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}
MEMO_SET_NOTIFY_ON
	ログオン時と、あなた宛のメモが到着した時に、%s が新着メモを通知します。
MEMO_SET_NOTIFY_LOGON
	ログオン時と、/AWAY を解除した時に、%s が新着メモを通知します。
MEMO_SET_NOTIFY_NEW
	メモが到着した時に、%s が新着メモを通知します。
MEMO_SET_NOTIFY_OFF
	%s はメモの着信をお知らせしません。
MEMO_SET_NOTIFY_MAIL
	電子メールによる新着メモの通知を有効にしました。
MEMO_SET_NOTIFY_NOMAIL
	電子メールによる新着メモの通知を無効にしました。
MEMO_SET_NOTIFY_INVALIDMAIL
	あなたのニックネームに対する電子メールアドレスが見つかりません。

# SET LIMIT responses
MEMO_SET_LIMIT_SYNTAX
	SET LIMIT [channel] limit
MEMO_SET_LIMIT_SERVADMIN_SYNTAX
	SET LIMIT [user | channel] {limit | NONE} [HARD]
MEMO_SET_YOUR_LIMIT_FORBIDDEN
	あなたはメモ保存量の上限値を変える許可を持っていません。
MEMO_SET_LIMIT_FORBIDDEN
	%s のメモ保存量の上限値は変わりませんでした。
MEMO_SET_YOUR_LIMIT_TOO_HIGH
	メモ保存量の上限値を %d 以上にすることはできません。
MEMO_SET_LIMIT_TOO_HIGH
	%s のメモ保存量の上限値を %d 以上にすることはできません。
MEMO_SET_LIMIT_OVERFLOW
	メモ保存量の上限値が大きすぎたため、%d に制限しました。
MEMO_SET_YOUR_LIMIT
	あなたのメモ保存量の上限を %d にしました。
MEMO_SET_YOUR_LIMIT_ZERO
	これ以上メモを受け取ることができません。
MEMO_UNSET_YOUR_LIMIT
	メモ保存量の上限値が無効になりました。
MEMO_SET_LIMIT
	%s のメモ保存量の上限が %d になりました。
MEMO_SET_LIMIT_ZERO
	%s のメモ保存量の上限が 0 になりました。
MEMO_UNSET_LIMIT
	%s のメモ保存量の上限値が無効になりました。

# INFO responses
MEMO_INFO_SYNTAX
	INFO [channel]
MEMO_INFO_SERVADMIN_SYNTAX
	INFO [nick | channel]
MEMO_INFO_NO_MEMOS
	今のところ、あなた宛のメモはありません。
MEMO_INFO_MEMO
	あなた宛のメモが 1 通あります。
MEMO_INFO_MEMO_UNREAD
	あなた宛のメモが 1 通あり、未読です。
MEMO_INFO_MEMOS
	あなた宛のメモが %d 通あります。
MEMO_INFO_MEMOS_ONE_UNREAD
	あなた宛のメモが %d 通あり、内 1 通が未読です。
MEMO_INFO_MEMOS_SOME_UNREAD
	あなた宛のメモが %d 通あり、内 %d 通が未読です。
MEMO_INFO_MEMOS_ALL_UNREAD
	あなた宛のメモが %d 通あり、全て未読です。
MEMO_INFO_LIMIT
	あなた宛のメモ保存量の上限値は %d です。
MEMO_INFO_HARD_LIMIT
	あなた宛のメモ保存量の上限値は %d で変えられません。
MEMO_INFO_LIMIT_ZERO
	あなた宛のメモ保存量の上限値が 0 なのでメモを受け取ることができない状態です。
MEMO_INFO_HARD_LIMIT_ZERO
	あなた宛のメモ保存量の上限値が 0 なのでメモを受け取ることができない状態です。また、メモの上限値を変えることは出来ません。
MEMO_INFO_NO_LIMIT
	あなた宛のメモ保存量の上限値が在りません。
MEMO_INFO_NOTIFY_OFF
	あなた宛の新着メモが到着してもお知らせしません。
MEMO_INFO_NOTIFY_ON
	あなた宛の新着メモの到着をログオン時と受信時にお知らせします。
MEMO_INFO_NOTIFY_RECEIVE
	あなた宛の新着メモの到着を受信時にお知らせします。
MEMO_INFO_NOTIFY_SIGNON
	あなた宛の新着メモの到着をログオン時にお知らせします。
MEMO_INFO_X_NO_MEMOS
	今のところ %s はメモを受け取っていません。
MEMO_INFO_X_MEMO
	%s 宛てのメモが 1 通あります。
MEMO_INFO_X_MEMO_UNREAD
	%s 宛のメモが 1 通あり、未読です。
MEMO_INFO_X_MEMOS
	%s 宛のメモが %d 通あります。
MEMO_INFO_X_MEMOS_ONE_UNREAD
	%s 宛のメモが %d 通あり、内 1 通が未読です。
MEMO_INFO_X_MEMOS_SOME_UNREAD
	%s 宛のメモが %d 通あり、内 %d 通が未読です。
MEMO_INFO_X_MEMOS_ALL_UNREAD
	%s 宛のメモが %d 通あり、全て未読です。
MEMO_INFO_X_LIMIT
	%s 宛のメモ保存量の上限値は %d です。
MEMO_INFO_X_HARD_LIMIT
	%s 宛のメモ保存量の上限値は %d で変わりません。
MEMO_INFO_X_NO_LIMIT
	%s 宛てのメモ保存量の上限値が在りません。
MEMO_INFO_X_NOTIFY_OFF
	%s 宛の新着メモを受け取ってもお知らせしません。
MEMO_INFO_X_NOTIFY_ON
	%s 宛の新着メモの到着をログオン時と受信時にお知らせします。
MEMO_INFO_X_NOTIFY_RECEIVE
	%s 宛の新着メモの到着を受信時にお知らせします。
MEMO_INFO_X_NOTIFY_SIGNON
	%s 宛の新着メモの到着をログオン時にお知らせします。

# Memo2Mail responses
MEMO_MAIL_SUBJECT
	新着メモ
MEMO_MAIL_TEXT1
	やぁ %s
MEMO_MAIL_TEXT2
	%s からの新着メモが到着しました。メモ番号は %d です。
MEMO_MAIL_TEXT3
	メモの内容:

# RSEND responses
MEMO_RSEND_PLEASE_WAIT
	RSEND コマンドを使う前に %d 秒待ってください。
MEMO_RSEND_DISABLED
	ご迷惑をかけて申し訳ございません。RSEND コマンドをこのネットワークで使うことはできません。
MEMO_RSEND_SYNTAX
	RSEND {nick | channel} memo-text
MEMO_RSEND_NICK_MEMO_TEXT
	[auto-memo] あなたが送信したメモが開封されました。
MEMO_RSEND_CHAN_MEMO_TEXT
	[auto-memo] あなたが %s 宛てに送信したメモが開封されました。
MEMO_RSEND_USER_NOTIFICATION
	あなたが %s からのメモを読んだことを
	お知らせするメモが送信されました。

# CHECK responses
MEMO_CHECK_SYNTAX
	CHECK nickname
MEMO_CHECK_NOT_READ
	あなたが %s 宛てに %s に送信した最新のメモは未開封のままです。
MEMO_CHECK_READ
	あなたが %s 宛てに%s に送信した最新のメモが開封されました。
MEMO_CHECK_NO_MEMO
	あなたはニックネーム %s 宛てのメモを送信していません。
MEMO_NO_RSEND_SELF
	自分から自分宛てのメモをに対する開封確認をすることはできません。

###########################################################################
#
# BotServ messages
#
###########################################################################
	
# Standard responses
BOT_DOES_NOT_EXIST
	ボット %s はいません。
BOT_NOT_ASSIGNED
	このコマンドを使う前に、ボットをチャンネルに召喚しなければいけません。
	より詳細なヘルプは、%R%S HELP ASSIGN で見る事ができます。
BOT_NOT_ON_CHANNEL
	チャンネル %s にボットはいません。
	
# Kick reasons (must be a single line)
BOT_REASON_BADWORD
	このチャンネルで "%s" という単語を使わないでください!
BOT_REASON_BADWORD_GENTLE
	言葉に気をつけてください!
BOT_REASON_BOLD
	このチャンネルでは太字を使わないでください!
BOT_REASON_CAPS
	caps lock を OFF にしてください!
BOT_REASON_COLOR
	このチャンネルでは色付き文字を使わないでください!
BOT_REASON_FLOOD
	flood 攻撃を止めてください!
BOT_REASON_REPEAT
	何回も同じ発言をしないでください!
BOT_REASON_REVERSE
	このチャンネルでは反転文字を使わないでください!
BOT_REASON_UNDERLINE
	このチャンネルでは下線を使わないでください!
	
# !seen replies
BOT_SEEN_BOT
	見つかっちゃったね。%s!
BOT_SEEN_YOU
	自分を探してるのかい %s?
BOT_SEEN_ON_CHANNEL
	たった今、%s がこのチャンネルに参加しました!
BOT_SEEN_ON_CHANNEL_AS
	たった今、%s が %s としてこのチャンネルに参加しました!
BOT_SEEN_ON
	%s がこのチャンネルにいたのは %s 前です。
BOT_SEEN_NEVER
	%s は今までこのチャンネルに参加したことがないようです。
BOT_SEEN_UNKNOWN
	%s って誰ですか?

# BOT responses
BOT_BOT_SYNTAX
	BOT ADD nick user host real
	BOT CHANGE oldnick newnick [user [host [real]]]
	BOT DEL nick
BOT_BOT_ALREADY_EXISTS
	ボット %s は既にこのサーバにいます。
BOT_BOT_CREATION_FAILED
	ご迷惑をかけて申し訳ございません。ボット召喚に失敗しました。
BOT_BOT_READONLY
	ご迷惑をかけて申し訳ございません。ボット召喚に失敗しました。
BOT_BOT_ADDED
	%s!%s@%s (%s) がボットリストに追加されました。
BOT_BOT_ANY_CHANGES
	info に変化がありませんでした。
BOT_BOT_CHANGED
	ボット %s は %s!%s@%s (%s) に変わりました。
BOT_BOT_DELETED
	ボット %s は 削除されました。
	
# BOTLIST responses
BOT_BOTLIST_HEADER
	ボットリスト:
BOT_BOTLIST_PRIVATE_HEADER
	オペレータに管理されているボット:
BOT_BOTLIST_FOOTER
	%d 個のボットが活動中です。
BOT_BOTLIST_EMPTY
	現在活動中のボットはいません。サーバオペレータに
	1 つボットを作るように要請してください!
	
# ASSIGN responses
BOT_ASSIGN_SYNTAX
	ASSIGN chan nick
BOT_ASSIGN_READONLY
	ご迷惑をおかけして申し訳ございません。一時的にボットの召喚ができなくなっています。
BOT_ASSIGN_ALREADY
	ボット %s は既にチャンネル %s に召喚されています。
BOT_ASSIGN_ASSIGNED
	ボット %s が %s に召喚されました。
	
# UNASSIGN responses
BOT_UNASSIGN_SYNTAX
	UNASSIGN chan
BOT_UNASSIGN_UNASSIGNED
	%s からボットを削除しました。
	
# INFO responses
BOT_INFO_SYNTAX
	INFO {chan | nick}
BOT_INFO_NOT_FOUND
	%s は正しいボットの名前でないか、このボットが登録されたチャンネルではありません。
BOT_INFO_BOT_HEADER
	ボット %s の情報:
BOT_INFO_BOT_MASK
	     マスク : %s@%s
BOT_INFO_BOT_REALNAME
	       実名 : %s
BOT_INFO_BOT_CREATED
	   召喚日時 : %s
BOT_INFO_BOT_USAGE
	     使用数 : %d チャンネル
BOT_INFO_BOT_OPTIONS
	 オプション : %s
BOT_INFO_OPT_PRIVATE
	プライベート
BOT_INFO_CHAN_HEADER
	チャンネル %s の情報:
BOT_INFO_CHAN_BOT
	ボットのニックネーム : %s
BOT_INFO_CHAN_BOT_NONE
	ボットのニックネーム : 設定されていません
BOT_INFO_CHAN_KICK_BADWORDS
	 禁止ワードキッカー : %s
BOT_INFO_CHAN_KICK_BADWORDS_BAN
	 禁止ワードキッカー : %s (キック回数がこれを超えたらバン: %d 回)
BOT_INFO_CHAN_KICK_BOLDS
	       太字キッカー : %s
BOT_INFO_CHAN_KICK_BOLDS_BAN
	       太字キッカー : %s (キック回数がこれを超えたらバン: %d 回)
BOT_INFO_CHAN_KICK_CAPS_ON
	     大文字キッカー : %s (最低 %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_BAN
	     大文字キッカー : %s (キック回数がこれを超えたらバン: %d 回; 最低 %d/%d%%)
BOT_INFO_CHAN_KICK_CAPS_OFF
	     大文字キッカー : %s
BOT_INFO_CHAN_KICK_COLORS
	     色文字キッカー : %s
BOT_INFO_CHAN_KICK_COLORS_BAN
	     色文字キッカー : %s (キック回数がこれを超えたらバン: %d 回)
BOT_INFO_CHAN_KICK_FLOOD_ON
	 Flood 攻撃キッカー : %s (%d 回の書き込みを %d 秒以内に行うとキック)
BOT_INFO_CHAN_KICK_FLOOD_BAN
	 Flood 攻撃キッカー : %s (キック回数がこれを超えたらバン: %d 回; %d 回の書き込みを %d 秒以内に行うとキック)
BOT_INFO_CHAN_KICK_FLOOD_OFF
	 Flood 攻撃キッカー : %s
BOT_INFO_CHAN_KICK_REPEAT_ON
	   繰り返しキッカー : %s (%d 回でキック)
BOT_INFO_CHAN_KICK_REPEAT_BAN
	   繰り返しキッカー : %s (キック回数がこれを超えたらバン: %d 回; %d 回でキック)
BOT_INFO_CHAN_KICK_REPEAT_OFF
	   繰り返しキッカー : %s
BOT_INFO_CHAN_KICK_REVERSES
	   反転表示キッカー : %s
BOT_INFO_CHAN_KICK_REVERSES_BAN
	   反転表示キッカー : %s (キック回数がこれを超えたらバン: %d 回)
BOT_INFO_CHAN_KICK_UNDERLINES
	       下線キッカー : %s
BOT_INFO_CHAN_KICK_UNDERLINES_BAN
	       下線キッカー : %s (キック回数がこれを超えたらバン: %d 回)
BOT_INFO_ACTIVE
	有効
BOT_INFO_INACTIVE
	無効
BOT_INFO_CHAN_OPTIONS
	         オプション : %s
BOT_INFO_OPT_DONTKICKOPS
	オペレータ保護
BOT_INFO_OPT_DONTKICKVOICES
	Voices ユーザ保護
BOT_INFO_OPT_FANTASY
	ファンタジー
BOT_INFO_OPT_GREET
	歓迎
BOT_INFO_OPT_NOBOT
	ボット禁止
BOT_INFO_OPT_SYMBIOSIS
	Symbiosis
BOT_INFO_OPT_NONE
	未設定
	
# SET responses
BOT_SET_SYNTAX
	SET (channel | bot) option settings
BOT_SET_DISABLED
	ご迷惑をかけて申し訳ありません。ボット機能に対するオプション操作は一時的に使うことができない状態になっています。
BOT_SET_UNKNOWN
	未定義のオプション: %s。
	より詳細なヘルプは、%R%S HELP SET で見る事ができます。
	
# SET DONTKICKOPS messages
BOT_SET_DONTKICKOPS_SYNTAX
	SET channel DONTKICKOPS {ON|OFF}
BOT_SET_DONTKICKOPS_ON
	ボットはチャンネル %s からオペレータをキックしません。
BOT_SET_DONTKICKOPS_OFF
	ボットはチャンネル %s からオペレータをキックします。
	
# SET DONTKICKVOICES messages
BOT_SET_DONTKICKVOICES_SYNTAX
	SET channel DONTKICKVOICES {ON|OFF}
BOT_SET_DONTKICKVOICES_ON
	ボットはチャンネル %s から voice ユーザをキックしません。
BOT_SET_DONTKICKVOICES_OFF
	ボットはチャンネル %s から voice ユーザをキックします。

# SET FANTASY messages
BOT_SET_FANTASY_SYNTAX
	SET channel FANTASY {ON|OFF}
BOT_SET_FANTASY_ON
	チャンネル %s でファンタジーモードが有効化されました。
BOT_SET_FANTASY_OFF
	チャンネル %s でファンタジーモードが無効化されました。
	
# SET GREET messages
BOT_SET_GREET_SYNTAX
	SET channel GREET {ON|OFF}
BOT_SET_GREET_ON
	チャンネル %s で歓迎モードが有効化されました。
BOT_SET_GREET_OFF
	チャンネル %s で歓迎モードが無効化されました。
	
# SET NOBOT
BOT_SET_NOBOT_SYNTAX
	SET botname NOBOT {ON|OFF}
BOT_SET_NOBOT_ON
	チャンネル %s でボット禁止モードが有効化されました。
BOT_SET_NOBOT_OFF
	チャンネル %s でボット禁止モードが無効化されました。

# SET PRIVATE
BOT_SET_PRIVATE_SYNTAX
	SET botname PRIVATE {ON|OFF}
BOT_SET_PRIVATE_ON
	ボット %s のプライベートモードが有効化されました。
BOT_SET_PRIVATE_OFF
	ボット %s のプライベートモードが無効化されました。

# SET SYMBIOSIS
BOT_SET_SYMBIOSIS_SYNTAX
	SET channel SYMBIOSIS {ON|OFF}
BOT_SET_SYMBIOSIS_ON
	チャンネル %s で Symbiosis モードが有効化されました。
BOT_SET_SYMBIOSIS_OFF
	チャンネル %s で Symbiosis モードが無効化されました。
	
# KICK responses
BOT_KICK_SYNTAX
	KICK channel option {ON|OFF} [settings]
BOT_KICK_DISABLED
	ご迷惑をかけて申し訳ありません。キック設定は一時的にすることができない状態になっています。
BOT_KICK_UNKNOWN
	未定義のオプション: %s。
	より詳細なヘルプは、%R%S HELP KICK で見る事ができます。
BOT_KICK_BAD_TTB
	%s はバンする時間と解釈出来ません。

BOT_KICK_BADWORDS_ON
	ボットは禁止ワードに反応してキックを行います。
	BADWORD コマンドで禁止ワードの追加と削除が行えます。
BOT_KICK_BADWORDS_ON_BAN
	ボットは禁止ワードに反応してキックを行い、
	同一ユーザのキックが %d 回になったところでバンします。
	BADWORD コマンドで禁止ワードの追加と削除が行えます。
BOT_KICK_BADWORDS_OFF
	ボットは禁止ワードの発言に対してキックを行いません。
BOT_KICK_BOLDS_ON
	ボットは太文字発言に対してキックを行います。
BOT_KICK_BOLDS_ON_BAN
	ボットは太文字発言に対してキックを行い、
	同一ユーザのキックが %d 回になったところでバンします。
BOT_KICK_BOLDS_OFF
	ボットは太文字発言に対してキックを行いません。
BOT_KICK_CAPS_ON
	ボットは大文字発言に対してキックを行います。
	(大文字が少なくとも %d 文字以上で発言全体の文字数に対する割合が %d%% 以上)
BOT_KICK_CAPS_ON_BAN
	ボットは大文字発言に対してキックを行い、
	(大文字が少なくとも %d 文字以上で発言全体の文字数に対する割合が %d%% 以上)
	同一ユーザのキックが %d 回になったところでバンします。
BOT_KICK_CAPS_OFF
	ボットは大文字発言に対してキックを行いません。
BOT_KICK_COLORS_ON
	ボットは色文字発言に対してキックを行います。
BOT_KICK_COLORS_ON_BAN
	ボットは色文字発言に対してキックを行い、
	同一ユーザのキックが %d 回になったところでバンします。
BOT_KICK_COLORS_OFF
	ボットは色文字発言に対してキックを行いません。
BOT_KICK_FLOOD_ON
	ボットは flood 攻撃に対してキックを行います。(%d 回の発言を %d 秒以内に繰り返したら)
BOT_KICK_FLOOD_ON_BAN
	ボットは flood 攻撃に対してキックを行い、(%d 回の発言を %d 秒以内に繰り返したら)
	同一ユーザのキックが %d 回になったところでバンします。
BOT_KICK_FLOOD_OFF
	ボットは flood 攻撃に対してキックを行いません。
BOT_KICK_REPEAT_ON
	ボットは同一内容の発言に対してキックを行います
	(%d 回 繰り返して同じ内容を発言した場合)。
BOT_KICK_REPEAT_ON_BAN
	ボットは同一内容の発言に対してキックを行い、(%d 回
	繰り返して同じ内容を発言した場合) 同一ユーザのキックが %d 回
	になったところでバンします。
BOT_KICK_REPEAT_OFF
	ボットは同一内容の発言に対してキックを行いません。
BOT_KICK_REVERSES_ON
	ボットは反転文字発言に対してキックを行います。
BOT_KICK_REVERSES_ON_BAN
	ボットは反転文字発言に対してキックを行い、同一ユーザのキックが %d 回
	になったところでバンします。
BOT_KICK_REVERSES_OFF
	ボットは反転文字発言に対してキックを行いません。
BOT_KICK_UNDERLINES_ON
	ボットは下線発言に対してキックを行います。
BOT_KICK_UNDERLINES_ON_BAN
	ボットは下線発言に対してキックを行い、同一ユーザのキックが %d 回
	になったところでバンします。
BOT_KICK_UNDERLINES_OFF
	ボットは下線発言に対してキックを行いません。
	
# BADWORDS messages
BOT_BADWORDS_SYNTAX
	BADWORDS channel {ADD|DEL|LIST|CLEAR} [word | entry-list] [SINGLE|START|END]
BOT_BADWORDS_DISABLED
	ご迷惑をかけて申し訳ありません。禁止ワードリストの編集は一時的にすることができない状態になっています。
BOT_BADWORDS_REACHED_LIMIT
	ご迷惑をかけて申し訳ありません。禁止ワードリストの登録数は %d 個に制限されています。
BOT_BADWORDS_ALREADY_EXISTS
	%s は既に %s の禁止ワードリストに登録されています。
BOT_BADWORDS_ADDED
	%s が %s の禁止ワードリストに登録されました。
BOT_BADWORDS_NO_SUCH_ENTRY
	エントリ (#%d) は %s の禁止ワードリストに登録されていません。
BOT_BADWORDS_NOT_FOUND
	%s は %s の禁止ワードリストに登録されていません。
BOT_BADWORDS_NO_MATCH
	該当するエントリが %s の禁止ワードリストに登録されていません。
BOT_BADWORDS_DELETED
	%s が %s の禁止ワードリストから削除されました。
BOT_BADWORDS_DELETED_ONE
	1 個のエントリが %s の禁止ワードリストから削除されました。
BOT_BADWORDS_DELETED_SEVERAL
	%d 個のエントリが %s の禁止ワードリストから削除されました。
BOT_BADWORDS_LIST_EMPTY
	%s の禁止ワードリストに登録されているエントリはありません。
BOT_BADWORDS_LIST_HEADER
	%s の禁止ワードリスト:
	  番号  ワード                         タイプ
BOT_BADWORDS_LIST_FORMAT
	  %3d   %-30s %s
BOT_BADWORDS_CLEAR
	禁止ワードリストに登録されているエントリはありません。

# SAY responses
BOT_SAY_SYNTAX
	SAY channel text

# ACT responses
BOT_ACT_SYNTAX
	ACT channel text

# Errors
BOT_EXCEPT
	チャンネル除外にマッチするユーザ
BOT_BAD_NICK
	ボットのニックネームにはニックネームでの使用が許可されている文字を使ってください。
BOT_BAD_HOST
	ボットのホストネームにはホストネームでの使用が許可されている文字を使ってください。
BOT_BAD_IDENT
	ボットの Idents には Idents での使用が許可されている文字を使ってください。
BOT_LONG_IDENT
	ボットの Idents には %d 文字以上必要です。
BOT_LONG_HOST
	ボットのホストネームには %d 文字以上必要です。

###########################################################################
#
# OperServ messages
#
###########################################################################

# General messages
OPER_BOUNCY_MODES
	モードを変えることができません。サーバの設定を見直してください。
OPER_BOUNCY_MODES_U_LINE
	モードを変えることができません。サーバの設定項目の U:lines の項目を見直してください。

# GLOBAL responses
OPER_GLOBAL_SYNTAX
	GLOBAL message

# STATS responses
OPER_STATS_UNKNOWN_OPTION
	未定義の STATS オプション: %s。
OPER_STATS_CURRENT_USERS
	現在のユーザ数: %d (オペレータ数 %d)
OPER_STATS_MAX_USERS
	ユーザ数の上限: %d (%s)
# これは冗長ですが、多言語対応(名詞の複数変化)のためです。
# 以下のすべての項目に秒のフィールドを含めることができることに注意してください。
# 以下の設定では、uptime が 1 時間を越えると秒の代わりに分を表示するようになります。
# しかし、%d のフィールドを追加することで秒を表示することもできます。
OPER_STATS_UPTIME_DHMS
	サービス継続時間 %d 日 %02d:%02d
OPER_STATS_UPTIME_1DHMS
	サービス継続時間 %d 日 %02d:%02d
OPER_STATS_UPTIME_HMS
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_HM1S
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_H1MS
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_H1M1S
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_1HMS
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_1HM1S
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_1H1MS
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_1H1M1S
	サービス継続時間 %d 時間 %d 分
OPER_STATS_UPTIME_MS
	サービス継続時間 %d 分 %d 秒
OPER_STATS_UPTIME_M1S
	サービス継続時間 %d 分 %d 秒
OPER_STATS_UPTIME_1MS
	サービス継続時間 %d 分 %d 秒
OPER_STATS_UPTIME_1M1S
	サービス継続時間 %d 分 %d 秒
OPER_STATS_BYTES_READ
	読み取りバイト: %5d kB
OPER_STATS_BYTES_WRITTEN
	書き込みバイト: %5d kB
OPER_STATS_USER_MEM
	ユーザ数    : %6d レコード、%5d kB
OPER_STATS_CHANNEL_MEM
	チャンネル数  : %6d レコード、%5d kB
OPER_STATS_GROUPS_MEM
	NS グループ数 : %6d レコード、%5d kB
OPER_STATS_ALIASES_MEM
	NS エイリアス数:%6d レコード、%5d kB
OPER_STATS_CHANSERV_MEM
	ChanServ      : %6d レコード、%5d kB
OPER_STATS_BOTSERV_MEM
	BotServ       : %6d レコード、%5d kB
OPER_STATS_HOSTSERV_MEM
	HostServ      : %6d レコード、%5d kB
OPER_STATS_OPERSERV_MEM
	OperServ      : %6d レコード、%5d kB
OPER_STATS_SESSIONS_MEM
	セッション数  : %6d レコード、%5d kB
OPER_STATS_AKILL_COUNT
	現在までに AKILL した数   : %d
OPER_STATS_AKILL_EXPIRE_DAYS
	AKILL 失効時間の初期値    : %d 日
OPER_STATS_AKILL_EXPIRE_DAY
	AKILL 失効時間の初期値    : 1 日
OPER_STATS_AKILL_EXPIRE_HOURS
	AKILL 失効時間の初期値    : %d 時間
OPER_STATS_AKILL_EXPIRE_HOUR
	AKILL 失効時間の初期値    : 1 時間
OPER_STATS_AKILL_EXPIRE_MINS
	AKILL 失効時間の初期値    : %d 分
OPER_STATS_AKILL_EXPIRE_MIN
	AKILL 失効時間の初期値    : 1 分
OPER_STATS_AKILL_EXPIRE_NONE
	AKILL 失効時間の初期値    : 未設定
OPER_STATS_SGLINE_COUNT
	現在の SGLINE 数          : %d
OPER_STATS_SGLINE_EXPIRE_DAYS
	SGLINE 失効時間の初期値   : %d 日
OPER_STATS_SGLINE_EXPIRE_DAY
	SGLINE 失効時間の初期値   : 1 日
OPER_STATS_SGLINE_EXPIRE_HOURS
	SGLINE 失効時間の初期値   : %d 時間
OPER_STATS_SGLINE_EXPIRE_HOUR
	SGLINE 失効時間の初期値   : 1 時間
OPER_STATS_SGLINE_EXPIRE_MINS
	SGLINE 失効時間の初期値   : %d 分
OPER_STATS_SGLINE_EXPIRE_MIN
	SGLINE 失効時間の初期値   : 1 分
OPER_STATS_SGLINE_EXPIRE_NONE
	SGLINE 失効時間の初期値   : 未設定
OPER_STATS_SQLINE_COUNT
	現在の SQLINE 数          : %d
OPER_STATS_SQLINE_EXPIRE_DAYS
	SQLINE 失効時間の初期値   : %d 日
OPER_STATS_SQLINE_EXPIRE_DAY
	SQLINE 失効時間の初期値   : 1 日
OPER_STATS_SQLINE_EXPIRE_HOURS
	SQLINE 失効時間の初期値   : %d 時間
OPER_STATS_SQLINE_EXPIRE_HOUR
	SQLINE 失効時間の初期値   : 1 時間
OPER_STATS_SQLINE_EXPIRE_MINS
	SQLINE 失効時間の初期値   : %d 分
OPER_STATS_SQLINE_EXPIRE_MIN
	SQLINE 失効時間の初期値   : 1 分
OPER_STATS_SQLINE_EXPIRE_NONE
	SQLINE 失効時間の初期値   : 未設定
OPER_STATS_SZLINE_COUNT
	現在の SZLINE 数          : %d
OPER_STATS_SZLINE_EXPIRE_DAYS
	SZLINE 失効時間の初期値   : %d 日
OPER_STATS_SZLINE_EXPIRE_DAY
	SZLINE 失効時間の初期値   : 1 日
OPER_STATS_SZLINE_EXPIRE_HOURS
	SZLINE 失効時間の初期値   : %d 時間
OPER_STATS_SZLINE_EXPIRE_HOUR
	SZLINE 失効時間の初期値   : 1 時間
OPER_STATS_SZLINE_EXPIRE_MINS
	SZLINE 失効時間の初期値   : %d 分
OPER_STATS_SZLINE_EXPIRE_MIN
	SZLINE 失効時間の初期値   : 1 分
OPER_STATS_SZLINE_EXPIRE_NONE
	SZLINE 失効時間の初期値   : 未設定
OPER_STATS_RESET
	設定がリセットされました。
OPER_STATS_UPLINK_SERVER
	アップリンクサーバ: %s
OPER_STATS_UPLINK_CAPAB
	アップリンク capab: %s
OPER_STATS_UPLINK_SERVER_COUNT
	アップリンクサーバ数: %d

# MODE responses
OPER_MODE_SYNTAX
	MODE channel modes

# UMODE respondes
OPER_UMODE_SYNTAX
	UMODE nick modes
OPER_UMODE_SUCCESS
	%s のユーザモードを変更しました。
OPER_UMODE_CHANGED
	%s があなたのユーザモードを変更しました。

# OLINE responses
OPER_OLINE_SYNTAX
	OLINE nick flags
OPER_OLINE_SUCCESS
	オペレータフラグ %s が %s に設定されました。
OPER_OLINE_IRCOP
	あなたは現在 IRC オペレータです。

# CLEARMODES responses
OPER_CLEARMODES_SYNTAX
	CLEARMODES channel [ALL]
OPER_CLEARMODES_DONE
	チャンネル %s のバイナリモードと ban リストが初期化されました。
OPER_CLEARMODES_ALL_DONE
	チャンネル %s のすべてのモードが初期化されました。


# KICK responses
OPER_KICK_SYNTAX
	KICK channel user reason

# SVSNICK responses
OPER_SVSNICK_SYNTAX
	SVSNICK nick newnick
OPER_SVSNICK_NEWNICK
	%s のニックネームが %s に変わりました。
	
# ADMIN responses
OPER_ADMIN_SYNTAX
	ADMIN {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_ADMIN_SKELETON
	サービスはスケルトンモードになっています。ADMIN コマンドは使えません。
OPER_ADMIN_EXISTS
	%s は既にサービス管理者リストに登録されています。
OPER_ADMIN_REACHED_LIMIT
	ご迷惑をかけて申し訳ありません。あなたは %d 個以上のサービス管理者にはなれません。
OPER_ADMIN_ADDED
	%s がサービス管理者リストに登録されました。
OPER_ADMIN_NOT_FOUND
	%s はサービス管理者リストに登録されていません。
OPER_ADMIN_NO_MATCH
	サービス管理者リストに該当するエントリが見つかりませんでした。
OPER_ADMIN_DELETED
	%s がサービス管理者リストから削除されました。
OPER_ADMIN_DELETED_ONE
	1 個のエントリがサービス管理者リストから削除されました。
OPER_ADMIN_DELETED_SEVERAL
	%d 個のエントリがサービス管理者リストから削除されました。
OPER_ADMIN_LIST_EMPTY
	サービス管理者リストが空です。
OPER_ADMIN_LIST_HEADER
	サービス管理者リスト:
	  番号  ニックネーム
OPER_ADMIN_LIST_FORMAT
	  %3d   %s
OPER_ADMIN_CLEAR
	サービス管理者リストが空になりました。
OPER_ADMIN_MOVED
	%s がサービス管理者リストに移動されました。

# OPER responses
OPER_OPER_SYNTAX
	OPER {ADD|DEL|LIST|CLEAR} [nick | entry-list]
OPER_OPER_SKELETON
	サービスはスケルトンモードになっています。OPER コマンドは使えません。
OPER_OPER_EXISTS
	%s は既にサービスオペレータリストに登録されています。
OPER_OPER_REACHED_LIMIT
	ご迷惑をかけて申し訳ありません。あなたは %d 個以上のサービスオペレータにはなれません。
OPER_OPER_ADDED
	%s がサービスオペレータリストに登録されました。
OPER_OPER_NOT_FOUND
	%s はサービスオペレータリストに登録されていません。
OPER_OPER_NO_MATCH
	サービスオペレータリストに該当するエントリが見つかりませんでした。
OPER_OPER_DELETED
	%s がサービスオペレータリストから削除されました。
OPER_OPER_DELETED_ONE
	1 個のエントリがサービスオペレータリストから削除されました。
OPER_OPER_DELETED_SEVERAL
	%d 個のエントリがサービスオペレータリストから削除されました。
OPER_OPER_LIST_EMPTY
	サービスオペレータリストが空です。
OPER_OPER_LIST_HEADER
	サービスオペレータリスト:
	  番号  ニックネーム
OPER_OPER_LIST_FORMAT
	  %3d   %s
OPER_OPER_CLEAR
	サービスオペレータリストが空になりました。
OPER_OPER_MOVED
	%s がサービスオペレータリストに移動されました。

# AKILL responses
OPER_AKILL_SYNTAX
	AKILL {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_AKILL_EXISTS
	%s は既に AKILL リストに登録されています。
OPER_AKILL_ALREADY_COVERED
	%s は登録済みのマスク %s にマッチします。
OPER_AKILL_REACHED_LIMIT
	ご迷惑をかけて申し訳ございません。AKILL リスト登録数の上限は %d です。
OPER_AKILL_NO_NICK
	リマインダ: AKILL マスクにニックネームを使うことはできません。マスクにニックネームが含まれていないか確認してください。
OPER_AKILL_ADDED
	%s が AKILL リストに登録されました。
OPER_AKILL_CHANGED
	%s の AKILL 失効時間が変更されました。
OPER_AKILL_NOT_FOUND
	%s は AKILL リストに登録されていません。
OPER_AKILL_NO_MATCH
	AKILL リストに該当するエントリが見つかりませんでした。
OPER_AKILL_DELETED
	%s が AKILL リストから削除されました。
OPER_AKILL_DELETED_ONE
	1 個のエントリが AKILL リストから削除されました。
OPER_AKILL_DELETED_SEVERAL
	%d 個のエントリが AKILL リストから削除されました。
OPER_AKILL_LIST_EMPTY
	AKILL リストが空です。
OPER_AKILL_LIST_HEADER
	現在の AKILL リスト:
	  番号  マスク                            理由
OPER_AKILL_LIST_FORMAT
	  %3d   %-32s  %s
OPER_AKILL_VIEW_HEADER
	現在の AKILL リスト:
# number, mask, set-by, set-time, expires, reason
OPER_AKILL_VIEW_FORMAT
	%3d  %s (by %s on %s; %s)
	      %s
OPER_AKILL_CLEAR
	AKILL リストが空になりました。
OPER_CHANKILL_SYNTAX
	CHANKILL [+expiry] {#channel} [reason]
# SGLINE responses
OPER_SGLINE_SYNTAX
	SGLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list}[:reason]]
OPER_SGLINE_UNSUPPORTED
	ご迷惑をかけて申し訳ありません。SGLINE はこのネットワークでは使えません。
OPER_SGLINE_EXISTS
	%s は既に SGLINE リストに登録されています。
OPER_SGLINE_ALREADY_COVERED
	%s は登録済みのマスク %s にマッチします。
OPER_SGLINE_REACHED_LIMIT
	ご迷惑をかけて申し訳ございません。SGLINE リスト登録数の上限は %d です。
OPER_SGLINE_ADDED
	%s が SGLINE リストに登録されました。
OPER_SGLINE_CHANGED
	%s の SGLINE 失効時間が変更されました。
OPER_SGLINE_NOT_FOUND
	%s は SGLINE リストに登録されていません。
OPER_SGLINE_NO_MATCH
	SGLINE リストに該当するエントリが見つかりませんでした。
OPER_SGLINE_DELETED
	%s が SGLINE リストから削除されました。
OPER_SGLINE_DELETED_ONE
	1 個のエントリが SGLINE リストから削除されました。
OPER_SGLINE_DELETED_SEVERAL
	%d 個のエントリが SGLINE リストから削除されました。
OPER_SGLINE_LIST_EMPTY
	SGLINE リストが空です。
OPER_SGLINE_LIST_HEADER
	現在の SGLINE リスト:
	  番号  マスク                            理由
OPER_SGLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SGLINE_VIEW_HEADER
	現在の SGLINE リスト:
# number, mask, set-by, set-time, expires, reason
OPER_SGLINE_VIEW_FORMAT
	%3d  %s (登録者 %s 登録日時 %s; %s)
	      %s
OPER_SGLINE_CLEAR
	SGLINE リストが空になりました。

# SQLINE responses
OPER_SQLINE_SYNTAX
	SQLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SQLINE_CHANNELS_UNSUPPORTED
	チャンネル SQLINE は御使用中の IRC デーモンではサポートされていません。そのためこの機能を使うことはできません。
OPER_SQLINE_EXISTS
	%s は既に SQLINE リストに登録されています。
OPER_SQLINE_ALREADY_COVERED
	%s は登録済みのマスク %s にマッチします。
OPER_SQLINE_REACHED_LIMIT
	ご迷惑をかけて申し訳ございません。SQLINE リスト登録数の上限は %d です。
OPER_SQLINE_ADDED
	%s が SQLINE リストに登録されました。
OPER_SQLINE_CHANGED
	%s の SQLINE 失効時間が変更されました。
OPER_SQLINE_NOT_FOUND
	%s は SQLINE リストに登録されていません。
OPER_SQLINE_NO_MATCH
	SQLINE リストに該当するエントリが見つかりませんでした。
OPER_SQLINE_DELETED
	%s が SQLINE リストから削除されました。
OPER_SQLINE_DELETED_ONE
	1 個のエントリが SQLINE リストから削除されました。
OPER_SQLINE_DELETED_SEVERAL
	%d 個のエントリが SQLINE リストから削除されました。
OPER_SQLINE_LIST_EMPTY
	SQLINE リストが空です。
OPER_SQLINE_LIST_HEADER
	現在の SQLINE リスト:
	  番号  マスク                            理由
OPER_SQLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SQLINE_VIEW_HEADER
	現在の SQLINE リスト:
# number, mask, set-by, set-time, expires, reason
OPER_SQLINE_VIEW_FORMAT
	%3d  %s (登録者 %s 登録日時 %s; %s)
	      %s
OPER_SQLINE_CLEAR
	SQLINE リストが空になりました。

# SZLINE responses
OPER_SZLINE_SYNTAX
	SZLINE {ADD | DEL | LIST | VIEW | CLEAR} [[+expiry] {mask | entry-list} [reason]]
OPER_SZLINE_UNSUPPORTED
	ご迷惑をかけて申し訳ありません。SZLINE はこのネットワークでは使えません。
OPER_SZLINE_EXISTS
	%s は既に SZLINE リストに登録されています。
OPER_SZLINE_ALREADY_COVERED
	%s は登録済みのマスク %s にマッチします。
OPER_SZLINE_REACHED_LIMIT
	ご迷惑をかけて申し訳ございません。SZLINE リスト登録数の上限は %d です。
OPER_SZLINE_ONLY_IPS
	リマインダ: SZLINE リストには IP アドレスマスク以外は登録できません。
OPER_SZLINE_ADDED
	%s が SZLINE リストに登録されました。
OPER_SZLINE_CHANGED
	%s の SZLINE 失効時間が変更されました。
OPER_SZLINE_NOT_FOUND
	%s は SZLINE リストに登録されていません。
OPER_SZLINE_NO_MATCH
	SZLINE リストに該当するエントリが見つかりませんでした。
OPER_SZLINE_DELETED
	%s が SZLINE リストから削除されました。
OPER_SZLINE_DELETED_ONE
	1 個のエントリが SZLINE リストから削除されました。
OPER_SZLINE_DELETED_SEVERAL
	%d 個のエントリが SZLINE リストから削除されました。
OPER_SZLINE_LIST_EMPTY
	SZLINE リストが空です。
OPER_SZLINE_LIST_HEADER
	現在の SZLINE リスト:
	  番号  マスク                            理由
OPER_SZLINE_LIST_FORMAT
	  %3d   %-32s  %s
OPER_SZLINE_VIEW_HEADER
	現在の SZLINE リスト:
# number, mask, set-by, set-time, expires, reason
OPER_SZLINE_VIEW_FORMAT
	%3d  %s (登録者 %s 登録日時 %s; %s)
	      %s
OPER_SZLINE_CLEAR
	SZLINE リストが空になりました。

# SET responses
OPER_SET_SYNTAX
	SET option setting
OPER_SET_IGNORE_ON
	無視コードを使用します。
OPER_SET_IGNORE_OFF
	無視コードを使用しません。
OPER_SET_IGNORE_ERROR
	IGNORE の setting 引数は ON か OFF のどちらかにしてください。
OPER_SET_READONLY_ON
	サービスは読取専用モードになっています。
OPER_SET_READONLY_OFF
	サービスは読み書きモードになっています。
OPER_SET_READONLY_ERROR
	READONLY の setting 引数は ON か OFF のどちらかにしてください。
OPER_SET_LOGCHAN_ON
	チャンネルログを %s に出力します。
OPER_SET_LOGCHAN_OFF
	チャンネルログを出力しません。
OPER_SET_LOGCHAN_ERROR
	LOGCHAN の setting 引数は ON か OFF のどちらかにし、LogChannel を定義してください。
OPER_SET_DEBUG_ON
	サービスをデバッグモードに設定しました。
OPER_SET_DEBUG_OFF
	サービスのデバッグモードを解除しました。
OPER_SET_DEBUG_LEVEL
	サービスをデバッグモード (デバッグレベル %d) に設定しました。
OPER_SET_DEBUG_ERROR
	DEBUG の setting 引数は ON か OFF か正数のどれかにしてください。
OPER_SET_NOEXPIRE_ON
	サービスの expire モードを解除しました。
OPER_SET_NOEXPIRE_OFF
	サービスを expire モードに設定しました。
OPER_SET_NOEXPIRE_ERROR
	NOEXPIRE の setting 引数は ON か OFF のどちらかにしてください。
OPER_SET_UNKNOWN_OPTION
	未定義のオプション: %s。
OPER_SET_SQL_ON
	SQL コードを使用します。
OPER_SET_SQL_OFF
	SQL コードを使用しません。
OPER_SET_SQL_ERROR
	SQL の setting 引数は ON か OFF のどちらかにしてください。
OPER_SET_SQL_ERROR_INIT
	SQL の初期化中にエラーが発生しました。解決のためにログを確認してください。
OPER_SET_SQL_ERROR_DISABLED
	このネットワークでは SQL が設定されていません。
	設定ファイル (services.conf) を編集してください。
OPER_SET_LIST_OPTION_ON
	%s が有効化されました。
OPER_SET_LIST_OPTION_OFF
	%s が無効化されました。

# NOOP responses
OPER_NOOP_SYNTAX
	NOOP {SET|REVOKE} server
OPER_NOOP_SET
	サーバ %s に対するすべての O:lines が削除されました。
OPER_NOOP_REVOKE
	サーバ %s に対するすべての O:lines がリセットされました。

# JUPE responses
OPER_JUPE_SYNTAX
	JUPE servername [reason]
OPER_JUPE_HOST_ERROR
	正しいホスト名を使ってください。
OPER_JUPE_INVALID_SERVER
	あなた自身のサーバあなたのアップリンクサーバには jupe できません。
	
# RAW responses
OPER_RAW_SYNTAX
	RAW text

# UPDATE responses
OPER_UPDATING
	データベースを更新しています。
	
# RELOAD responses
OPER_RELOAD
	サービスの設定ファイルのリロードを行いました。

# RESTART responses
OPER_CANNOT_RESTART
	SERVICES_BIN が未定義のため再起動できません。\2設定\2スクリプトを再度走らせて、RESTART コマンドを有効化させるためにサービスを再コンパイルしてください。

# IGNORE respondes
OPER_IGNORE_SYNTAX
	IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
OPER_IGNORE_VALID_TIME
	time 引数に有効な数字を設定してください。
OPER_IGNORE_TIME_DONE
	%s は設定済みのマスク %s にマッチします。
OPER_IGNORE_PERM_DONE
	%s を永久的に無視します。
OPER_IGNORE_DEL_DONE
	%s を無視しません。
OPER_IGNORE_LIST
	無視リスト:
OPER_IGNORE_LIST_NOMATCH
	ニックネーム %s は無視リストに登録されていません。
OPER_IGNORE_LIST_EMPTY
	無視リストが空です。
OPER_IGNORE_LIST_CLEARED
	無視リストが空になりました。

# KILLCLONES responses
OPER_KILLCLONES_SYNTAX
	KILLCLONES nick
OPER_KILLCLONES_UNKNOWN_NICK
	ニックネーム %s のユーザが見つかりませんでした。
	
# CHANLIST responses
OPER_CHANLIST_HEADER
	チャンネルリスト:
	チャンネル名         ユーザモード  トピック
OPER_CHANLIST_HEADER_USER
	ユーザ %s のチャンネルリスト:
	チャンネル名         ユーザモード  トピック
OPER_CHANLIST_RECORD
	%-20s  %4d +%-6s %s
OPER_CHANLIST_END
	チャンネルリストは以上です。
	
# USERLIST responses
OPER_USERLIST_HEADER
	ユーザリスト:
	ニックネーム         マスク
OPER_USERLIST_HEADER_CHAN
	チャンネル %s のユーザリスト:
	ニックネーム         マスク
OPER_USERLIST_RECORD
	%-20s %s@%s
OPER_USERLIST_END
	ユーザリストは以上です。

OPER_SUPER_ADMIN_ON
	あなたは現在 SuperAdmin となっています。
OPER_SUPER_ADMIN_OFF
	あなたは SuperAdmin ではなくなりました。
OPER_SUPER_ADMIN_SYNTAX
	SuperAdmin の setting 引数は ON か OFF のどちらか にしてください (services.conf の中で設定)
OPER_SUPER_ADMIN_WALL_ON
	%s が現在の Super-Admin です。
OPER_SUPER_ADMIN_WALL_OFF
	%s は Super-Admin ではありません。
OPER_SUPER_ADMIN_ONLY
	Super-Admin だけがこのコマンドを使えます。
OPER_STAFF_LIST_HEADER
	On レベル ニックネーム
OPER_STAFF_FORMAT
	 %c %s  %s
OPER_STAFF_AFORMAT
	 %c %s  %s [%s]

# DefCon Messages

OPER_DEFCON_SYNTAX
	DEFCON [1|2|3|4|5]
OPER_DEFCON_DENIED
	サービスが Defcon モードになっています。後からやり直してください。
OPER_DEFCON_NO_CONF
	Defcon システムは services.conf 内でのみ有効化できます。
OPER_DEFCON_CHANGED
	DEFCON レベルが変更されました。現在のレベルは %d です。
OPER_DEFCON_WALL
	%s が DEFCON レベルを %d に変更しました。
DEFCON_GLOBAL
	現在の DEFCON レベル: %d

# Module strings

OPER_MODULE_LOADED
	モジュール %s がロードされました。
OPER_MODULE_UNLOADED
	モジュール %s がアンロードされました。
OPER_MODULE_LOAD_FAIL
	モジュール %s がロードできませんでした。
OPER_MODULE_REMOVE_FAIL
	モジュール %s がアンロードできませんでした。
OPER_MODULE_NO_UNLOAD
	このモジュールはアンロードできません。
OPER_MODULE_LOAD_SYNTAX
	MODLOAD FileName
OPER_MODULE_UNLOAD_SYNTAX
	MODUNLOAD FileName
OPER_MODULE_LIST_HEADER
	現在のモジュールリスト:
OPER_MODULE_LIST
	モジュール: %s [%s] [%s]
OPER_MODULE_LIST_FOOTER
	%d 個のモジュールがロードされています。
OPER_MODULE_INFO_LIST
	モジュール: %s バージョン: %s 作者: %s ロード: %s
OPER_MODULE_CMD_LIST
	使用可能なコマンド: %R%s %s
OPER_MODULE_MSG_LIST
	使用可能な IRC デーモンハンドラ: %s
OPER_MODULE_NO_LIST
	モジュールが 1 つもロードされていません。
OPER_MODULE_NO_INFO
	モジュール %s についての情報がありません。
OPER_MODULE_INFO_SYNTAX
	MODINFO FileName
MODULE_HELP_HEADER
	以下のコマンドはこのモジュールによってロードされています:

###########################################################################
#
# Session Limiting and Exception messages
#
###########################################################################

# EXCEPTION responses
OPER_EXCEPTION_SYNTAX
	EXCEPTION {ADD | DEL | MOVE | LIST | VIEW} [params]
OPER_EXCEPTION_ADD_SYNTAX
	EXCEPTION ADD [+expiry] mask limit reason
OPER_EXCEPTION_DEL_SYNTAX
	EXCEPTION DEL {mask | list}
OPER_EXCEPTION_MOVE_SYNTAX
	EXCEPTION MOVE num position
OPER_EXCEPTION_DISABLED
	セッション数制限機能が無効化されています。
OPER_EXCEPTION_ALREADY_PRESENT
	マスク %s は既に除外リストに登録済みです。
OPER_EXCEPTION_TOO_MANY
	セッション数制限からの除外リストが満杯です!
OPER_EXCEPTION_ADDED
	%s に対するセッション数の上限値が %d に設定されました。
OPER_EXCEPTION_MOVED
	%s (#%d) に対する除外リストの番号が %d に変わりました。
OPER_EXCEPTION_NO_SUCH_ENTRY
	セッション数制限からの除外リストに #%d というエントリはありません。
OPER_EXCEPTION_NOT_FOUND
	セッション数制限からの除外リストに %s というエントリはありません。
OPER_EXCEPTION_NO_MATCH
	セッション数制限からの除外リストに該当するエントリはありません。
OPER_EXCEPTION_DELETED
	セッション数制限からの除外リストから %s というエントリを削除しました。
OPER_EXCEPTION_DELETED_ONE
	セッション数制限からの除外リストから 1 個のエントリを削除しました。
OPER_EXCEPTION_DELETED_SEVERAL
	セッション数制限からの除外リストから %d 個のエントリを削除しました。
OPER_EXCEPTION_LIST_HEADER
	現在のセッション数制限からの除外リスト:
# mask, limit
OPER_EXCEPTION_LIST_FORMAT
	%3d  %4d   %s
OPER_EXCEPTION_LIST_COLHEAD
	番号 上限値 ホストマスク
# mask, set-by, set-time, expires, limit, reason
OPER_EXCEPTION_VIEW_FORMAT
	%3d.  %s  (by %s on %s; %s)
	    リミット: %-4d  - %s
OPER_EXCEPTION_INVALID_LIMIT
	無効なセッション数の上限値です。零以上で %d より小さな整数を設定してください。
OPER_EXCEPTION_INVALID_HOSTMASK
	無効なホストマスクです。単純なホストマスクのみが有効で、マッチングはニックネームやユーザネームに対しては行われません。
OPER_EXCEPTION_EXISTS
	%s は既にセッション数制限からの除外リストに登録済みです。
OPER_EXCEPTION_CHANGED
	%s に対するセッション数の上限値が %d に変更されました。

# SESSION responses
OPER_SESSION_SYNTAX
	SESSION {LIST limit | VIEW host}
OPER_SESSION_LIST_SYNTAX
	SESSION LIST limit
OPER_SESSION_VIEW_SYNTAX
	SESSION VIEW host
OPER_SESSION_DISABLED
	セッション数制限機能が無効化されています。
OPER_SESSION_INVALID_THRESHOLD
	無効な上限値です。1 より大きい整数を指定してください。
OPER_SESSION_NOT_FOUND
	%s はセッションリストにありません。
OPER_SESSION_LIST_HEADER
	セッション数が %d 以上のホスト:
OPER_SESSION_LIST_COLHEAD
	セッション数 ホスト名
# limit, host
OPER_SESSION_LIST_FORMAT
	%6d    %s
# host, sessions, limit
OPER_SESSION_VIEW_FORMAT
	現在、ホスト名 %s のセッション数は %d で上限値が %d に設定されています。

# EXCEPTION help

OPER_HELP_EXCEPTION
	構文: EXCEPTION ADD [+expiry] mask limit reason
	      EXCEPTION DEL {mask | list}
	      EXCEPTION MOVE num position
	      EXCEPTION LIST [mask | list]
	      EXCEPTION VIEW [mask | list]
	
	サービス管理者はセッション数制限からの除外ホスト名のリストを編集できます。
	このリストには、各ホストに対するセッション数の上限値が記述 されています。
	たとえばシェルサーバのようなマシンからのセッション数の上限値を初期値
	から増やすことができます。あるホストからのセッション数が上限値に達すると、
	以降にこのホストから接続試行を行ったクライアントは kill されます。
	セッション数制限機能の円滑な運用のために、このようなユーザが kill
	される前に、%S からの /NOTICE で上限に達したことが通知されます。
	この通知の内容は設定ファイルで指定できます。
	
	EXCEPTION ADD は与えられたホストマスクを除外リストに追加します。
	注意する点は、nick!user@host や user@host のようなマスク
	は無効だという点です。 例えば、box.host.dom や *.host.dom
	のような単純なホストマスクのみが有効です。その理由は、セッション数制限
	はニックネームやユーザネームを考慮しないからです。また、limit は
	0 以上の数字でなければいけません。この引数はマスクで指定したホスト群
	から一度に受け付けるセッション数の上限値を示しています。ただし、0 を
	指定した場合はセッション数制限が無いという意味になります。
	AKILL のヘルプをご覧になって、任意で指定する expiry
	パラメータのフォーマットに関するより詳しい情報をご確認ください。
	EXCEPTION DEL は引数で指定されたマスクを除外リストから削除します。
	EXCEPTION MOVE は num 番目の除外エントリを position
	番目のエントリに移動します。2 つのエントリ間にあるエントリは
	順番を保存したまま上下にシフトします。
	EXCEPTION LIST と EXCEPTION VIEW はすべての除外リストを
	表示します。mask 引数を指定した場合は、これにマッチする
	除外エントリのみを表示します。2 つの違いは、EXCEPTION VIEW
	の方が詳細 (除外エントリを設定した人の名前、
	セッション数の上限値、その理由、ホストマスク、失効日時を表示します。)
	という点です。
	
	注意する点は、クライアントの接続試行に対して除外リストの上から 1
	つづつマッチングを行うという点です。そのため、大きな除外リストや
	広範囲にマッチする除外マスクはサービスのパフォーマンスを低下させます。

OPER_HELP_SESSION
	構文: SESSION LIST threshold
	      SESSION VIEW host
	
	サービス管理者はセッションリストを見ることができます。

	SESSION LIST は threshold 以下のセッション数を使っている
	ホストを表示します。この値は 1 よりも大きい数でなければいけません。
	これは、間違えて 1 を指定した結果、大量にリストアップされてしまう
	ことを防ぐためです。
	SESSION VIEW はあるホストについて詳細な情報を表示します。
	これには、セッション数とセッション数の上限値が含まれます。
	引数 host には、ワイルドカードを含めてはいけません。

	セッション数制限の概要と、あるホストかホストグループに対して
	セッション数制限を設定する方法については、EXCEPTION
	のヘルプも参照してください。

OPER_HELP_STAFF
	構文: STAFF

	サービススタッフ全員のニックネーム、権限レベル
	とオンラインか否かを表示します。

OPER_HELP_DEFCON
	構文: DEFCON [1|2|3|4|5]

	defcon システムはサービス対して、あらかじめ決められた設定で
	制限を掛けます。これはネットワークが攻撃を受けている最中に
	有用です。

OPER_HELP_DEFCON_NO_NEW_CHANNELS
	* 新規チャンネルの登録を禁止します。
OPER_HELP_DEFCON_NO_NEW_NICKS
	* 新規ニックネームの登録を禁止します。
OPER_HELP_DEFCON_NO_MLOCK_CHANGE
	* MLOCK の変更を禁止します。
OPER_HELP_DEFCON_FORCE_CHAN_MODES
	* 全てのチャンネルのチャンネルモードを (%s) に変更します。
OPER_HELP_DEFCON_REDUCE_SESSION
	* セッション数制限を %d に減らします。
OPER_HELP_DEFCON_NO_NEW_CLIENTS
	* 全ての新しいクライアントからの接続を Kill します。
OPER_HELP_DEFCON_OPER_ONLY
	* メッセージを出して、全てのオペレータでないユーザを無視します。
OPER_HELP_DEFCON_SILENT_OPER_ONLY
	* メッセージを出さずに、全てのオペレータでないユーザを無視します。
OPER_HELP_DEFCON_AKILL_NEW_CLIENTS
	* 新規クライアントからの接続を AKILL します。
OPER_HELP_DEFCON_NO_NEW_MEMOS
	* 新規メモの送信を禁止します。
OPER_HELP_CHANKILL
	構文: CHANKILL [+expiry] channel reason

	あるチャンネルに参加している全てのニックネームを AKILL
	リストに加える。これは、全てのニックネームに対して、完全な
	ident@host を使い、AKILL を実行する。

###########################################################################
#
# News system messages
#
###########################################################################

NEWS_LOGON_TEXT
	[ログオンニュース - %s] %s
NEWS_OPER_TEXT
	[オペレータニュース - %s] %s
NEWS_RANDOM_TEXT
	[ランダムニュース - %s] %s

NEWS_LOGON_SYNTAX
	LOGONNEWS {ADD|DEL|LIST} [text|num]
NEWS_LOGON_LIST_HEADER
	ログオンニュース:
NEWS_LOGON_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_LOGON_LIST_NONE
	ログオンニュースがありません。
NEWS_LOGON_ADD_SYNTAX
	LOGONNEWS ADD text
NEWS_LOGON_ADD_FULL
	ニュースリストが満杯です!
NEWS_LOGON_ADDED
	新しいログオンニュース (#%d) を追加しました。
NEWS_LOGON_DEL_SYNTAX
	LOGONNEWS DEL {num | ALL}
NEWS_LOGON_DEL_NOT_FOUND
	ログオンニュース #%d が見つかりません!
NEWS_LOGON_DELETED
	ログオンニュース #%d を削除しました。
NEWS_LOGON_DEL_NONE
	ログオンニュースを削除しませんでした!
NEWS_LOGON_DELETED_ALL
	ログオンニュースを全て削除しました。

NEWS_OPER_SYNTAX
	OPERNEWS {ADD|DEL|LIST} [text|num]
NEWS_OPER_LIST_HEADER
	サーバオペレータニュース:
NEWS_OPER_LIST_ENTRY
	%5d (%s by %s)
	    %s
NEWS_OPER_LIST_NONE
	サーバオペレータニュースがありません。
NEWS_OPER_ADD_SYNTAX
	OPERNEWS ADD text
NEWS_OPER_ADD_FULL
	ニュースリストが満杯です!
NEWS_OPER_ADDED
	新しいサーバオペレータニュース (#%d) を追加しました。
NEWS_OPER_DEL_SYNTAX
	OPERNEWS DEL {num | ALL}
NEWS_OPER_DEL_NOT_FOUND
	サーバオペレータニュース #%d が見つかりません!
NEWS_OPER_DELETED
	サーバオペレータニュース #%d を削除しました。
NEWS_OPER_DEL_NONE
	サーバオペレータニュースを削除できませんでした!
NEWS_OPER_DELETED_ALL
	サーバオペレータニュースを全て削除しました。

NEWS_RANDOM_SYNTAX
	RANDOMNEWS {ADD|DEL|LIST} [text|num]
NEWS_RANDOM_LIST_HEADER
	ランダムニュース:
NEWS_RANDOM_LIST_ENTRY
	%5d (%s 投稿者 %s)
	    %s
NEWS_RANDOM_LIST_NONE
	ランダムニュースがありません。
NEWS_RANDOM_ADD_SYNTAX
	RANDOMNEWS ADD text
NEWS_RANDOM_ADD_FULL
	ニュースリストが満杯です!
NEWS_RANDOM_ADDED
	新しいランダムニュース (#%d) を追加しました。
NEWS_RANDOM_DEL_SYNTAX
	RANDOMNEWS DEL {num | ALL}
NEWS_RANDOM_DEL_NOT_FOUND
	ランダムニュース #%d が見つかりません!
NEWS_RANDOM_DELETED
	ランダムニュース #%d を削除しました。
NEWS_RANDOM_DEL_NONE
	ランダムニュースを削除できませんでした!
NEWS_RANDOM_DELETED_ALL
	ランダムニュースを全て削除しました。

NEWS_HELP_LOGON
	構文: LOGONNEWS ADD text
	      LOGONNEWS DEL {num | ALL}
	      LOGONNEWS LIST
	
	ログオンニュースを編集または表示する。ユーザがネットワークに
	接続したときに、これらのメッセージがユーザに送信される
	(ただし、ユーザにこれを flood 攻撃と勘違いさせないために、
	%s 個以上のメッセージは送信されない。この場合、
	メッセージは新しいものから順に送信される)。
	NewsCount は services.conf 内で設定する。
	
	LOGONNEWS はサービス管理者以外は使えない。

NEWS_HELP_OPER
	構文: OPERNEWS ADD text
	      OPERNEWS DEL {num | ALL}
	      OPERNEWS LIST
	
	サーバオペレータニュースを編集または表示する。ユーザが
	サーバオペレータになる (/OPER コマンドで) と、これらのメッセージ
	がユーザに送信される。(ただし、ユーザにこれを flood 攻撃と勘違い
	させないために、%s 個以上のメッセージは送信されない。
	この場合、メッセージは新しいものから順に送信される)。
	NewsCount は services.conf 内で設定する。
	
	OPERNEWS はサービス管理者以外は使えない。

NEWS_HELP_RANDOM
	構文: RANDOMNEWS ADD text
	      RANDOMNEWS DEL {num | ALL}
	      RANDOMNEWS LIST
	
	ランダムニュースを編集または表示する。ユーザがネットワークに
	接続すると、ランダムニュースの中から 1 つ、ニュースが無作為に
	選ばれ、これが送信される。
	
	RANDOMNEWS はサービス管理者以外は使えない。

###########################################################################
#
# HelpServ help message
#
###########################################################################

HELP_HELP
	%S はサービスについての情報を伝えるためのサービスです。
	他のサービスクライアントについての HELP コマンドにより、
	ヘルプ項目を読むことができます。

	%R%s HELP
	     ニックネーム登録についての情報。

	%R%s HELP
	     ニックネームとチャンネルの登録及び
	     管理についての情報。

	%R%s HELP
	     現在接続していないユーザにメッセージを送る事についての情報。

HELP_HELP_BOT

	%R%s HELP
	     自分のチャンネルにボットを設置する事についての情報。

HELP_HELP_HOST

	%R%s HELP
	     仮想ホストを設置する事についての情報。

###########################################################################
#
# NickServ help messages
#
###########################################################################

NICK_HELP_CMD_CONFIRM
	    CONFIRM    NickServ の認証コード (auth code) を確認する。
NICK_HELP_CMD_RESEND
	    RESEND     NickServ の認証コード (auth code) を再送りする。
NICK_HELP_CMD_REGISTER
	    REGISTER   ニックネームを登録する。
NICK_HELP_CMD_GROUP
	    GROUP      グループに加わる。
NICK_HELP_CMD_IDENTIFY
	    IDENTIFY   自分をパスワードで認証する。
NICK_HELP_CMD_ACCESS
	    ACCESS     許可されているアドレス表を編集する。
NICK_HELP_CMD_SET
	    SET        オプションの設定、殺し (KILL) 防衛を含む。
NICK_HELP_CMD_SASET
	    SASET      別のニックネームに SET オプションを設定する。
NICK_HELP_CMD_DROP
	    DROP       ニックネーム登録を解除する。
NICK_HELP_CMD_RECOVER
	    RECOVER    貴方のニックネームを使っているユーザを殺す (KILL)。
NICK_HELP_CMD_RELEASE
	    RELEASE    RECOVER の後でニックネームを取り戻す。
NICK_HELP_CMD_SENDPASS
	    SENDPASS   パスワードを登録してるメールアドレスに送る。
NICK_HELP_CMD_GHOST
	    GHOST      貴方のニックネームを使っている IRC「ゴースト」セッションを切断する。
NICK_HELP_CMD_ALIST
	    ALIST      貴方にアクセスが許可されてるチャンネルを表示する。
NICK_HELP_CMD_GLIST
	    GLIST      貴方のグループ内の全てのニックネームを表示する。
NICK_HELP_CMD_INFO
	    INFO       指定されたニックネームについての情報を表示する。
NICK_HELP_CMD_LIST
	    LIST       指定されたパターンに合う登録されてる全てのニックネームを表示する。
NICK_HELP_CMD_LOGOUT
	    LOGOUT     IDENTIFY コマンドの効果を取り消す。
NICK_HELP_CMD_STATUS
	    STATUS     指定されたニックネームの持ち主状態を出力する。
NICK_HELP_CMD_UPDATE
	    UPDATE     貴方の現在の状態を更新する。例: 新しいメモが届いたかを確認する。
NICK_HELP_CMD_GETPASS
	    GETPASS    指定されたニックネームのパスワードを出力する。
NICK_HELP_CMD_GETEMAIL
	    GETEMAIL   指定されたメールアドレスで登録されてる全てのユーザを出力する。
NICK_HELP_CMD_FORBID
	    FORBID     指定されたニックネームを登録禁止にする。
NICK_HELP_CMD_SUSPEND
	    SUSPEND    指定されたニックネームを使用禁止にする。
NICK_HELP_CMD_UNSUSPEND
	    UNSUSPEND  指定されたニックネームの使用禁止を解除する。

NICK_HELP
	%S を使うとニックネームの登録と、登録済みニックネーム
	を他の人に使われることを防ぐことが出来ます。
	ニックネームの登録とメンテナンスをするには、
	%R%S command と入力してください。
	より詳細なコマンドについての情報は
	%R%S HELP command と入力してください。
	
	
NICK_HELP_FOOTER
	 
	注意: このサービスを使うことで、IRC
	ユーザに自分の正体が乗っ取られる可能性を示唆できます。
	ただし、これはニックネームやその他の挙動を
	盗むことを抑制出来ません。
	%S を乱用することは少なくとも
	乱用されたニックネームの損失を招きます。
	
NICK_HELP_EXPIRES
	
	使用禁止ニックネームの登録は自動的に解除されます。
	たとえば、使用禁止ニックネームが使用されない場合には
	%d 日後に自動的に登録解除されます。

NICK_HELP_REGISTER
	構文: REGISTER password [email]
	
	%S のデータベースにニックネームを登録します。
	登録した後は、SET や ACCESS
	コマンドを使ってニックネームの設定を変更できます。
	登録時のパスワードを忘れないようにしてください。
	また、登録時のパスワードを変更することも忘れないでください。
	(大文字と小文字は区別されることに注意してください。
	ANOPE、Anope、anope これらはすべて異なるパスワードです!)
	
	パスワード選択に関するガイドライン:
	
	パスワードは簡単に推測できるものであってはなりません。例えば、
	あなたの実際の名前をパスワードにするのは良くありません。
	さらに、ニックネームをパスワードにするのは最悪ですし、
	%S はそれを許しません。
	また、短いパスワードは総当り検索に対して脆弱です。
	少なくとも 5 文字以上のパスワードを設定することをお勧めします。
	最後に、空白文字はパスワードとして使用できません。
	
	パラメータ email は任意で設定可能で、ニックネームに対する
	メールアドレスの紐付けが直ちに行われます。しかし、ネットワーク
	によってはこの設定が必須な場合があります。
	我々はあなたのプライバシーを尊重します。ここで設定した
	メールアドレスが第三者に譲渡されることはありません。
	
	このコマンドはあなたのニックネームに対する新しいグループを
	作成します。このグループには同じ設定を共有する他のニックネーム
	を登録することが出来ます。ここで設定とはメモや同じチャンネル
	に対する特権の事を指します。より詳しい情報は、
	%R%S HELP GROUP と入力してください。
	
NICK_HELP_GROUP
	構文: GROUP target password
	
	このコマンドで target ニックネームのグループにあなたの
	ニックネームを追加します。password はターゲットニックネームのパスワードです。
	
	グループに登録すると、
	グループに登録されているニックネームすべてに対して設定、
	メモ、チャンネル特権を共有できます!
	 
	グループは使用可能な状態であれば存在し続けます。これは、
	グループに含まれるニックネームがなくなったとしても、
	1 つ以上のニックネームがグループに含まれている限り、
	上で書いたような共有設定は失われないということです。
	
	ニックネームが登録されていない場合、このコマンドを使えます。
	ニックネームが登録されている場合は、このコマンドを使う前に
	認証が必要です。より詳しい情報は %R%S HELP IDENTIFY
	と入力してください。IRC ネットワークによっては
	このコマンドが使えない場合があります。
	
	このコマンドは未登録のニックネームと共に使うことをお勧めします。
	なぜなら、このコマンドを使うと自動的にニックネーム登録も
	されるからです。IRC ネットワークの管理者が許している場合に限り、
	登録済みのニックネームと共に使う (グループ変更する)
	ことも可能です。
	
	同時に複数ののグループに登録することは出来ません。また、
	グループの結合は出来ません。
	
	注意: あるグループに含まれるニックネームのパスワードはすべて同じです。

NICK_HELP_IDENTIFY
	構文: IDENTIFY password
	
	%S にあなたがこのニックネームの正統なオーナーであることを伝えます。
	このコマンドを使う前に多くのコマンドを使わなければいけません。
	パスワードは REGISTER コマンドで指定したものと同じでなければいけません。

NICK_HELP_UPDATE
	構文: UPDATE

	現在の状態を更新します。例えば、新しいメモをチェックしたり、
	必要なチャンネルモード (ModeonID) の設定を行ったり、仮想ホストや
	ユーザフラグ (最終更新時間) のアップデートをします。
	
NICK_HELP_LOGOUT
	構文: LOGOUT
	
	IDENTIFY コマンドの影響を逆転します。例えば、
	自分がニックネームの正統なユーザであることを認証されていない状態
	にします。再度認証するかどうかを尋ねられることはないという
	点にに注意してください。

NICK_HELP_DROP
	構文: DROP [nickname]
	
	ニックネームを %S のデータベースから削除します。
	削除されたニックネームを改めて登録することも出来ます。
	
	nick パラメータを渡すことで、
	あなたのグループからニックネームを削除できます。
	
	このコマンドを使うためには、まず自分の正統性を
	パスワードによって認証する必要があります (より詳しい
	情報は%R%S HELP IDENTIFY と入力してください)。

NICK_HELP_ACCESS
	構文: ACCESS ADD mask
	      ACCESS DEL mask
	      ACCESS LIST
	
	ニックネームへのアクセスリストを変更又は表示します。
	%S が自動的にこのニックネームを使用できると判断する
	アドレスのリストです。別のアドレスからこのニックネーム
	を使いたい場合は、IDENTIFY コマンドを使用して、
	%S にアドレスとニックネームの紐付けを行ってください。
	
	例:
	
	    ACCESS ADD anyone@*.bepeg.com
	        bepeg.com ドメインに属するマシンからの
	        ユーザ anyone のアクセスを許可します。
	
	    ACCESS DEL anyone@*.bepeg.com
	        上のコマンドを無効化します。
	
	    ACCESS LIST
	        現在のアクセスリストを表示します。

NICK_HELP_SET
	構文: SET option parameters
	
	ニックネームに関するオプションを設定する。option は以下の 1 つ:
	
	    DISPLAY    サービス内でのグループ表示状態の設定
	    PASSWORD   ニックネームパスワードの設定
	    LANGUAGE   サービスが送信するメッセージの
	                   表示言語の設定
	    URL        ニックネームと URL の紐付け
	    EMAIL      ニックネームと電子メールアドレスの紐付け
	    ICQ        ニックネームと ICQ 番号の紐付け
	    GREET      ニックネームと歓迎メッセージの紐付け
	    KILL       プロテクトの on と off の設定
	    SECURE     ニックネーム保護の on と off の設定
	    PRIVATE    ニックネームが %R%S LIST の中に
	                   現れないようにする。
	    HIDE       ニックネーム情報の一部を隠す
	    MSG        サービスとの通信方法を変更
	    AUTOOP     自動的にサービスオペレータ権をもらう
	
	このコマンドを使うためには、まず自分の正統性を
	パスワードによって認証する必要があります (より詳しい
	情報は %R%S HELP IDENTIFY と入力してください)。
	
	それぞれのオプションに関するより詳しい情報は、
	%R%S HELP SET option と入力してください。

NICK_HELP_SET_DISPLAY
	構文: SET DISPLAY new-display
	
	サービス内でのあなたのニックネームに関連付けられたグループ
	の表示状態を変更します。新しい表示はあなたのグループに登録された
	ニックネームでなければいけません。

NICK_HELP_SET_PASSWORD
	構文: SET PASSWORD new-password
	
	ニックネームのオーナーパスワードを変更します。

NICK_HELP_SET_LANGUAGE
	構文: SET LANGUAGE number
	
	サーバからのメッセージ表示言語を変更します (例えば、
	あなたからのコマンドのレスポンス等)。number は
	以下の言語リストから選択してください。
	

NICK_HELP_SET_URL
	構文: SET URL url
	
	ニックネームと URL を紐付けます。
	この URL は INFO コマンドで得られるニックネーム
	に関する情報に表示されます。

NICK_HELP_SET_EMAIL
	構文: SET EMAIL address
	
	ニックネームと電子メールアドレスを紐付けます。
	このアドレスは INFO コマンドで得られるニックネーム
	に関する情報に表示されます。
	
NICK_HELP_SET_ICQ
	構文: SET ICQ number
	
	ニックネームと ICQ 番号を紐付けます。
	この ICQ 番号は INFO コマンドで得られるニックネーム
	に関する情報に表示されます。
	
NICK_HELP_SET_GREET
	構文: SET GREET message
	
	ニックネームの歓迎メッセージを設定します。
	この歓迎メッセージは GREET オプションが有効化されたチャンネル
	に参加した際に表示され、あなたがチャンネルに必要なアクセス
	をもっている場合に供給されます。

NICK_HELP_SET_KILL
	構文: SET KILL {ON | QUICK | IMMED | OFF}
	
	あなたのニックネームに関する自動プロテクトの on と off を切り替えます。
	プロテクトが on の状態で、他のユーザがあなたのニックネームを
	使おうとした場合、他のユーザは %S がニックネームを強制
	変更するまでの 1 分間にニックネームを変更しなければなりません。
	
	QUICK を選んだ場合、通常 60 秒の猶予時間を 20 秒にします。
	IMMED を選んだ場合、警告無しにニックネームを強制変更
	します。このオプションは極力使わないでください。また、
	ネットワーク管理者がこれを使わせないようにしているかもしれません。

NICK_HELP_SET_SECURE
	構文: SET SECURE {ON | OFF}
	
	あなたのニックネームに対する %S の保護機能を on か off にします。
	SECURE を ON にすると、あなたニックネームのオーナー認証
	を行う前に、アクセスリスト上のあなたのアドレスの有無にかかわらず
	パスワードを入力しなければなりません。ただし、アクセスリスト
	にあなたのアドレスが有る場合は、%S は KILL オプションの
	設定にかかわらずあなたを自動的に KILL しません。

NICK_HELP_SET_PRIVATE
	構文: SET PRIVATE {ON | OFF}
	
	あなたのニックネームに関する %S のプライバシーオプションを
	on か off にします。PRIVATE を ON にすると、%S の LIST
	コマンドで表示されるニックネームリストに出てこなくなります
	(ただし、ニックネームを知っている人は、INFO コマンドで
	情報を見ることが出来ます)。

NICK_HELP_SET_HIDE
	構文: SET HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	あなたのニックネームに関する %S の INFO コマンドで表示される情報
	を制限できます。電子メールアドレス (EMAIL)、user@host
	マスク (USERMASK)、サービスアクセス状態 (STATUS)、最後の
	切断メッセージ (QUIT) を非表示に出来ます。
	2 番目のパラメータで表示するか (OFF)、隠すか (ON)
	を指定します。
	
NICK_HELP_SET_MSG
	構文: SET MSG {ON | OFF}
	
	あなたとサービスとの通信方法を選択できます。MSG がセットされると、
	サービスは messages を使用しますが、されなければ notices を使います。

NICK_HELP_SET_AUTOOP
	構文: SET AUTOOP {ON | OFF}
	
	あなたにオペレータ権を自動的に配布するかどうかを設定します。
	これが ON の場合、ChanServ はチャンネルに入った時に自動的に
	オペレータ権を配布します。

NICK_HELP_SASET
	構文: SASET nickname option parameters.
	
	ニックネームに関するオプションを設定する。option は以下の 1 つ:
	
	    DISPLAY    サービス内でのグループ表示状態の設定
	    PASSWORD   ニックネームパスワードの設定
	    URL        ニックネームと URL の紐付け
	    EMAIL      ニックネームと電子メールアドレスの紐付け
	    ICQ        ニックネームと ICQ 番号の紐付け
	    GREET      ニックネームと歓迎メッセージの紐付け
	    KILL       プロテクトの on と off の設定
	    SECURE     ニックネーム保護の on と off の設定
	    PRIVATE    ニックネームが %R%S LIST の中に
	                   現れないようにする。
	    HIDE       ニックネーム情報の一部を隠す
	    MSG        サービスとの通信方法を変更
	    NOEXPIRE   ニックネームの期限切れを無くす
	    AUTOOP     自動的にサービスオペレータ権をもらう
	    LANGUAGE   サービスからのメッセージの表示言語の設定
	
	それぞれのオプションに関するより詳しい情報は、
	%R%S HELP SASET option と入力してください。
	オプションは与えられた nickname に関して設定されます。

NICK_HELP_SASET_DISPLAY
	構文: SASET nickname DISPLAY new-display
	
	サービス内でのあなたのニックネームに関連付けられたグループ
	の表示状態を変更します。新しい表示はあなたのグループに登録された
	ニックネームでなければいけません。

NICK_HELP_SASET_PASSWORD
	構文: SASET nickname PASSWORD new-password
	
	ニックネームのオーナーパスワードを変更します。

NICK_HELP_SASET_URL
	構文: SASET nickname URL url
	
	ニックネームと URL を紐付けます。この URL は INFO コマンド
	で得られるニックネームに関する情報に表示されます。

NICK_HELP_SASET_EMAIL
	構文: SASET nickname EMAIL address
	
	ニックネームと電子メールアドレスを紐付けます。
	
NICK_HELP_SASET_ICQ
	構文: SASET nickname ICQ number
	
	ニックネームと ICQ 番号を紐付けます。
	この ICQ 番号は INFO コマンドで得られるニックネーム
	に関する情報に表示されます。
	
NICK_HELP_SASET_GREET
	構文: SASET nickname GREET message
	
	ニックネームの歓迎メッセージを設定します。
	この歓迎メッセージは GREET オプションが有効化されたチャンネル
	に参加した際に表示され、ユーザがチャンネルに必要なアクセス
	をもっている場合に供給されます。

NICK_HELP_SASET_KILL
	構文: SASET nickname KILL {ON | QUICK | IMMED | OFF}
	
	ニックネームに関する自動プロテクトの on と off を切り替えます。
	プロテクトが on の状態で、他のユーザがこのニックネームを
	使おうとした場合、他のユーザは %S がニックネームを強制
	変更するまでの 1 分間にニックネームを変更しなければなりません。
	
	QUICK を選んだ場合、通常 60 秒の猶予時間を 20 秒にします。
	IMMED を選んだ場合、警告無しにニックネームを強制変更
	します。このオプションは極力使わないでください。また、
	ネットワーク管理者がこれを使わせないようにしているかもしれません。

NICK_HELP_SASET_SECURE
	構文: SASET nickname SECURE {ON | OFF}
	
	このニックネームに対する %S の保護機能を on か off にします。
	SECURE を ON にすると、このニックネームのオーナー認証
	を行う前に、アクセスリスト上のユーザのアドレスの有無にかかわらず
	パスワードを入力しなければなりません。ただし、アクセスリスト
	にユーザのアドレスが有る場合は、%S は KILL オプションの
	設定にかかわらずユーザを自動的に KILL しません。

NICK_HELP_SASET_PRIVATE
	構文: SASET nickname PRIVATE {ON | OFF}
	
	このニックネームに関する %S のプライバシーオプションを
	on か off にします。PRIVATE を ON にすると、%S の LIST
	コマンドで表示されるニックネームリストに出てこなくなります
	(ただし、ニックネームを知っている人は、INFO コマンドで
	情報を見ることが出来ます)。

NICK_HELP_SASET_HIDE
	構文: SASET nickname HIDE {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}
	
	このニックネームに関する %S の INFO コマンドで表示される情報
	を制限できます。電子メールアドレス (EMAIL)、user@host
	マスク (USERMASK)、サービスアクセス状態 (STATUS)、最後の
	切断メッセージ (QUIT) を非表示に出来ます。
	2 番目のパラメータで表示するか (OFF)、隠すか (ON)
	を指定します。
	
NICK_HELP_SASET_MSG
	構文: SASET nickname MSG {ON | OFF}
	
	ユーザとサービスとの通信方法を選択できます。MSG がセットされると、
	サービスは messages を使用しますが、されなければ notices を使います。

NICK_HELP_SASET_NOEXPIRE
	構文: SASET nickname NOEXPIRE {ON | OFF}
	
	ニックネームを破棄するかどうか設定します。これを ON
	にすると、ニックネームは破棄されません。

NICK_HELP_SASET_AUTOOP
	構文: SASET nickname AUTOOP {ON | OFF}
	
	ユーザにオペレータ権を自動的に配布するかどうかを設定します。
	これが ON の場合、ChanServ はチャンネルに入った時に自動的に
	オペレータ権を配布します。

NICK_HELP_SASET_LANGUAGE
	構文: SASET nickname LANGUAGE number
	
	サーバから nickname に送信されるメッセージ表示言語を変更します
	(例えば、あなたからのコマンドのレスポンス等)。number は
	%R%S HELP SET LANGUAGE を入力して得られるリストから
	選択してください。

NICK_HELP_RECOVER
	構文: RECOVER nickname [password]
	
	もし、誰かがあなたのニックネームを使用している場合に、
	これを取り上げることが出来ます。この機能は、誰かが
	kill-保護されたニックネームを使おうとした場合に、
	%S がやることと同じです。
	
	このコマンドを使用すると、%S は保護したいニックネームと同じ
	ニックネームを使用する仮ユーザをネットワークに参加させます。
	こうすることで、IRC サーバは他のユーザからの接続を切断できます。
	%s が他のユーザからの再接続要求が無くなったと判断するまで、
	仮ユーザのオンライン状態は維持されます。この後に、あなたは
	自分のニックネームを再取得できます。別の方法として、
	RELEASE コマンド (%R%S HELP RELEASE) を使うことで、
	ニックネームをすばやく再取得できます。
	
	あるニックネームに対して RECOVER コマンドを使うためには、
	/WHOIS で表示される現在のアドレスがニックネームの
	アドレスリストとニックネームの所属するグループの
	アドレスリストに含まれるか、このニックネームの正しい
	パスワードを知っていなければいけません。

NICK_HELP_RELEASE
	構文: RELEASE nickname [password]
	
	%S が自動 kill 保護機能か RECOVER コマンドを使うことで
	使用中のあなたのニックネームを使えるようにします。
	%s の間ずっと試行を続けることですぐに使えるようになります。
	
	あるニックネームに対して RELEASE コマンドを使うためには、
	/WHOIS で表示される現在のアドレスがニックネームの
	アドレスリストとニックネームの所属するグループの
	アドレスリストに含まれるか、このニックネームの正しい
	パスワードを知っていなければいけません。

NICK_HELP_GHOST
	構文: GHOST nickname [password]
	
	あなたのニックネームを使用している「ゴースト」IRC セッション
	を終了します。「ゴースト」セッションとは、実際には接続
	していませんが、ニックネームを使用するユーザが何らかの
	理由でまだオンライン状態を保っていると IRC サーバに思わせる
	セッションのことです。このような状態は、典型的に言って、
	IRC サーバに接続中に、あなたのコンピュータがクラッシュするか、
	インターネットやモデムの接続が切れた場合に起きます。
	
	あるニックネームに対して GHOST コマンドを使うためには、
	/WHOIS で表示される現在のアドレスがニックネームの
	アドレスリストとニックネームの所属するグループの
	アドレスリストに含まれるか、このニックネームの正しい
	パスワードを知っていなければいけません。

NICK_HELP_INFO
	構文: INFO nickname [ALL]
	
	与えられたニックネームに関する情報を表示します。
	ニックネームのオーナー、最後にこのニックネームを使用した
	ユーザのアドレスと時間、ニックネームオプション。もし
	自分自身の情報を表示しようとして、かつ引数に ALL
	が与えられていた場合、隠しているかどうかにかかわらず、
	すべての情報を表示します。

NICK_HELP_LIST
	構文: LIST pattern
	
	nick!user@host 型のフォーマットに従って指定された
	パターンに一致するすべての登録済みニックネームを表示します。
	PRIVATE オプションが有効になっているニックネームは表示されません。
	パターンを '#' から始めることで特定の範囲を指定できます。
	
	例:
	
	    LIST *!joeuser@foo.com
	        joeuser@foo.com が所有するニックネームを表示。
	
	    LIST *Bot*!*@*
	        ニックネームに Bot (大文字小文字は区別される)
		が含まれる登録済みニックネームを表示。
	
	    LIST *!*@*.bar.org
	        bar.org ドメインに所属するユーザが所有する
		ニックネームを表示。
	
	    LIST #50-100
	        番号 50-100 までのニックネームを表示

NICK_HELP_ALIST
	構文: ALIST [level]
	
	あなたがアクセス中のすべてのチャンネルを表示します。引数に
	XOP か ACCESS コマンドと同じフォーマットのレベルを指定
	出来ます。与えられたアクセスレベルのチャンネルを表示します。

	例:
	    ALIST Founder
	        あなたが設立者のアクセス権を持つ
		チャンネルをすべて表示します。

	    ALIST AOP
	        あなたが AOP かそれ以上のアクセス権を持つ
		チャンネルをすべて表示します。

	    ALIST 10
		あなたが 10 以上のアクセス権を持つ
		チャンネルをすべて表示します。

	NOEXPIRE オプションが有効化されているチャンネルは
	感嘆符から始まるチャンネル名になります。

NICK_HELP_GLIST
	構文: GLIST
	
	あなたのグループに所属するすべてのニックネームを表示します。

NICK_HELP_STATUS
	構文: STATUS nickname...
	
	ニックネームを使用しているユーザがオーナーであるか
	確認します。レスポンスフォーマットは以下です。
	
	    nickname status-code
	
	ここで、nickname はコマンドで送られたニックネームで、
	status-code は以下の内の 1 つです。
	
	    0 - このニックネームを使用中のユーザがいない、又はニックネームが登録されていない。
	    1 - ユーザはニックネームのオーナーであるか確認できない。
	    2 - アクセスリストでユーザがオーナーであると判断された。
	    3 - パスワード認証でユーザがオーナーであると判断された。
	
	16 個までのニックネームを一度に送信できます。17 個目からは
	無視されます。ニックネームが与えられない場合、自分の
	ステータスが返されます。

NICK_HELP_SENDPASS
	構文: SENDPASS nickname
	
	指定されたニックネームのパスワードをニックネームレコードに
	登録済みのメールアドレスに送信します。このコマンドは
	判らなくなったパスワードを解決するのに役立ちます。
	
	一部のネットワークでは、IRC オペレータ だけがこのコマンドを使用できます。
	
	暗号化が有効化されている場合にはこのコマンドは使用できません。

NICK_HELP_CONFIRM
	構文: CONFIRM passcode
	
	これはニックネーム登録手順の 2 番目のステップです。
	%S に登録されたあなたのニックネームを使用するためにこのコマンド
	が必要です。認証の最初のステップでは、パスコード (認証コード) が
	あなたのメールアドレスに送信さます。認証の最初のステップに関する
	より詳しい情報は、%R%S HELP REGISTER と入力してください。

NICK_HELP_RESEND
	構文: RESEND
	
	このコマンドを発行したユーザのメールアドレス宛に、
	認証コード (パスコード) を再度送信します。

NICK_SERVADMIN_HELP
	 
	サービス管理者はニックネームオーナー認証無しにニックネーム
	登録を破棄できます。また、ニックネームのアクセスリスト
	を観覧することが出来ます (%R%S ACCESS LIST nick)。

NICK_SERVADMIN_HELP_LOGOUT
	構文: LOGOUT [nickname [REVALIDATE]]
	
	パラメータが無い場合、IDENTIFY コマンドの効果を逆転させます。
	例えば、あなたをこのニックネームの正統な所有者でなくします。
	また、再度認証を行うように聞かれることはありません。
	
	パラメータ有りの場合、与えられたニックネームに対して同様の
	操作を行います。REVALIDATE を引数に与えると、サービスは
	ユーザに再度認証を行うように聞きます。このコマンドを使うのは
	サービス管理者だけに制限されています。

NICK_SERVADMIN_HELP_DROP
	構文: DROP [nickname]
	
	パラメータ無しの場合、あなたのニックネームを
	%S のデータベースから削除します。
	
	パラメータ有りの場合、引数で指定されたニックネームをデータベース
	から削除します。あなたのグループ内であれば、どんなニックネームでも
	削除できます。自分の所属するグループにかかわらず、ニックネームを
	削除出来るのは、サービス管理者だけに限定されています。
	

NICK_SERVADMIN_HELP_INFO
	
	サービス管理者は ALL パラメータとニックネームを自由に使えます。

NICK_SERVADMIN_HELP_LIST
	構文: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE] [UNCONFIRMED]
	
	nick!user@host 型のフォーマットに従って指定された
	パターンに一致するすべての登録済みニックネームを表示します。
	PRIVATE オプションが有効になっているニックネームは
	サービス管理者に対してだけ表示されます。サービス管理者に対しては
	NOEXPIRE オプションが有効化されているニックネームには
	! が付加されます。
	
	FORBIDDEN、SUSPENDED、NOEXPIRE、UNCONFIRMED オプションが引数に
	指定されている場合、禁止、一時停止、未確認のニックネーム
	又は NOEXPIRE のフラグがセットされているニックネームが表示されます。
	複数のオプションが引数に指定された場合、少なくとも 1 つに
	該当するニックネームが表示されます。これらのオプションは
	サービス管理者だけが使用できます。

	例:
	
	    LIST *!joeuser@foo.com
	        joeuser@foo.com が所有する登録済みニックネームを表示。
	
	    LIST *Bot*!*@*
	        ニックネームに Bot (大文字小文字は区別される)
		が含まれる登録済みニックネームを表示。
	
	    LIST * NOEXPIRE
		登録破棄期限が設定されていない登録済みニックネームを
		表示。

NICK_SERVADMIN_HELP_ALIST
	構文: ALIST [nickname] [level]
	
	パラメータ無しの場合、アクセス中のチャンネルのリストを表示
	します。パラメータが 1 つの場合、nickname がアクセス
	しているチャンネルのリストを表示します。パラメータが 2 つの場合、
	nickname が level アクセスかそれ以上のアクセス権を
	持っているチャンネルのリストを表示します。

	このコマンドはサービス管理者だけが使えます。

NICK_SERVADMIN_HELP_GLIST
	構文: GLIST [nickname]
	
	パラメータ無しの場合、あなたのグループに所属する
	すべてのニックネームを表示します。
	
	パラメータ有りの場合、引数で指定されたニックネーム
	のグループに所属するすべてのニックネームを表示します。
	これはサービス管理者だけに制限されています。

NICK_SERVADMIN_HELP_GETPASS
	構文: GETPASS nickname
	
	引数で指定されたニックネームのパスワードを返します。
	注意するべきことは、このコマンドが使用されると、
	コマンドを発行したユーザと引数のニックネームが記録され、
	WALLOPS/GLOBOPS	宛に送信されます。
	
	このコマンドは暗号化が有効な場合には使用できません。

NICK_SERVADMIN_HELP_GETEMAIL
	構文: GETEMAIL user@emailhost

	引数で指定されたメールアドレスに該当するニックネームを
	返します。注意することは、user にも emailhost にもワイルドカード
	は使えないということです。このコマンドが使用されると、
	コマンドを発行したユーザと引数のメールアドレスが記録されます。

NICK_SERVADMIN_HELP_FORBID
	構文: FORBID nickname [reason]
	
	ニックネームが登録されないようにするか、使用中の
	状態にします。ニックネームの不正使用に有効かもしれません。
	
	一部のネットワークでは reason の指定が不可欠です。

NICK_SERVADMIN_HELP_SUSPEND
	構文: SUSPEND nickname reason

	あるニックネームを使用中から一時使用停止の状態に変化させます。

NICK_SERVADMIN_HELP_UNSUSPEND
	構文: UNSUSPEND nickname

	あるニックネームを使用中から使用可能の状態に変化させます。

###########################################################################
#
# ChanServ help messages
#
###########################################################################

CHAN_HELP_CMD_GETPASS
	    GETPASS    チャンネルの設立者パスワードを表示する。
CHAN_HELP_CMD_FORBID
	    FORBID     チャンネルを使用不可能にする。
CHAN_HELP_CMD_SUSPEND
	    SUSPEND    チャンネルデータと設定を保存した状態で
	               チャンネルを使用不可能 (一時的に使用不可) にする。
CHAN_HELP_CMD_UNSUSPEND
	    UNSUSPEND  一時使用不可のチャンネルを使用可能にする。
CHAN_HELP_CMD_STATUS
	    STATUS     あるチャンネルのにいるあるユーザの
	               現在のアクセスレベルを表示する。
CHAN_HELP_CMD_REGISTER
	    REGISTER   チャンネルを登録する。
CHAN_HELP_CMD_IDENTIFY
	    IDENTIFY   パスワードで自分の認証を行う。
CHAN_HELP_CMD_SET
	    SET        チャンネルオプションと情報の設定。
CHAN_HELP_CMD_AOP
	    AOP        AOP ユーザのリストを変更する。
CHAN_HELP_CMD_SOP
	    SOP        SOP ユーザのリストを変更する。
CHAN_HELP_CMD_ACCESS
	    ACCESS     特権ユーザのリストを変更する。
CHAN_HELP_CMD_LEVELS
	    LEVELS     アクセスレベルの定義を変更する。
CHAN_HELP_CMD_AKICK
	    AKICK      自動キックリストを操作する。
CHAN_HELP_CMD_DROP
	    DROP       チャンネル登録を解除する。
CHAN_HELP_CMD_SENDPASS
	    SENDPASS   紛失したパスワードを回復する。
CHAN_HELP_CMD_BAN
	    BAN        指定したニックネームをあるチャンネルからバンする。
CHAN_HELP_CMD_CLEAR
	    CLEAR      ChanServ に対してあるチャンネルのある設定を破棄させる。
CHAN_HELP_CMD_DEVOICE
	    DEVOICE    あるチャンネルで指定したニックネームを発言不可能にする。
CHAN_HELP_CMD_GETKEY
	    GETKEY     指定されたチャンネルのキーを返す。
CHAN_HELP_CMD_INFO
	    INFO       登録済みチャンネルの情報を返す。
CHAN_HELP_CMD_INVITE
	    INVITE     ChanServ に対してあるチャンネルに自分を招待させる。
CHAN_HELP_CMD_KICK
	    KICK       あるチャンネルから指定したニックネームをキックする。
CHAN_HELP_CMD_LIST
	    LIST       与えられたパターンに該当する登録済みチャンネルを返す。
CHAN_HELP_CMD_LOGOUT
	    LOGOUT     指定されたニックネームをログアウトさせる。
CHAN_HELP_CMD_OP
	    OP         あるチャンネルにいる指定されたニックネームの管理者状態を返す。
CHAN_HELP_CMD_TOPIC
	    TOPIC      指定されたチャンネルのトピックを変更する。
CHAN_HELP_CMD_UNBAN
	    UNBAN      自分が入れないあるチャンネルのバンをすべて削除する。
CHAN_HELP_CMD_VOICE
	    VOICE      あるチャンネルの指定されたニックネームに発言権を与える。
CHAN_HELP_CMD_VOP
	    VOP        あるチャンネルの VOP (発言可能なユーザ) リストを操作する
CHAN_HELP_CMD_DEHALFOP
	    DEHALFOP   あるチャンネルの指定されたニックネームから halfop 権を剥奪する。
CHAN_HELP_CMD_DEOWNER
	    DEOWNER    あるチャンネルのあなたから所有者状態を剥奪する。
CHAN_HELP_CMD_DEPROTECT
	    DEPROTECT  あるチャンネルの指定されたニックネームを保護しないようにする。
CHAN_HELP_CMD_HALFOP
	    HALFOP     あるチャンネルの指定されたニックネームに halfop 権を渡す。
CHAN_HELP_CMD_HOP
	    HOP        あるチャンネルの HOP (HalfOP) リストを操作する。
CHAN_HELP_CMD_OWNER
	    OWNER      所有者状態ををあなたに与える。
CHAN_HELP_CMD_PROTECT
	    PROTECT    あるチャンネルの指定されたニックネームを保護する。
CHAN_HELP_CMD_ADMIN
	    ADMIN      あるチャンネルの指定されたニックネームを保護する。
CHAN_HELP_CMD_DEADMIN
	    DEADMIN    あるチャンネルの指定されたニックネームを保護しないようにする。
CHAN_HELP_CMD_DEOP
	    DEOP       あるチャンネルの指定されたニックネームからオペレータ権を剥奪する。

CHAN_HELP
	%S はチャンネルの登録と設定を可能にします。
	%S にチャンネルオペレータ特権付加の登録を行うことで、
	悪意のあるユーザがチャンネルを乗っ取ることを防止します。
	使用できるコマンドは以下です。コマンドを使うには、
	%R%S command  と入力してください。それぞれの
	コマンドに対するより詳しい情報は %R%S HELP command
	 と入力してください。
	 
	
CHAN_HELP_EXPIRES
	
	%d 日以上使われていないチャンネル (例えば、チャンネルの
	アクセスリストに載っているユーザが 1 人もいない状態が
	この期間を超えた場合) は自動的に破棄されます。

CHAN_HELP_REGISTER
	構文: REGISTER channel password description
	
	%S のデータベースにチャンネルを登録する。このコマンドを使うには、
	登録したいチャンネルのチャンネルオペレータにならなければ
	いけません。他のユーザがこのチャンネルの設定を後から
	変更するためには、IDENTIFY コマンドと共にパスワードを
	使います。最後の引数は必須で、これはチャンネルの目的
	を説明します。
	
	チャンネルを登録するには、あなたはチャンネルの "founder"
	として登録済みでなければいけません。チャンネル設立者は、
	このチャンネルの設定を変更することが許されています。
	%S はチャンネル設立者がチャンネルに入ったときに
	チャンネルオペレータ権限を自動的に与えます。
	他のユーザに対するアクセス権限に関するより詳しい情報は
	ACCESS コマンド (%R%S HELP ACCESS)
	を参照してください。
	
	注意: チャンネルを登録する前に、あなたのニックネームが
	登録されていなければいけません。未登録の場合、
	%R%s HELP と入力して、ニックネーム登録方法を
	参照してください。

CHAN_HELP_IDENTIFY
	構文: IDENTIFY channel password
	
	引数で指定されたチャンネルの設立者権を %S から貰う。
	多くのコマンドがこれを使う前に必要です。パスワードは
	REGISTER コマンドで使用したものと同じです。
	
CHAN_HELP_LOGOUT
	構文: LOGOUT channel nickname
	
	このコマンドは指定されたニックネームをログアウトさせます。
	ログアウトさせられたユーザは再度認証をする必要があります。
	
	このチャンネルの設立者であれば、自分以外は誰でも強制的に
	ログアウトさせることが出来ます。

CHAN_HELP_DROP
	構文: DROP channel
	
	引数で指定されたチャンネルの登録を破棄します。
	これが出来るのは、IDENTIFY コマンドを使って認証済みの
	チャンネル設立者だけです。


CHAN_HELP_SET
	構文: SET channel option parameters
	
	チャンネル設立者がチャンネルオプションとその他の情報
	を設定できます。
	
	設定できるオプション:
	
	    FOUNDER       チャンネル設立者の設定
	    SUCCESSOR     チャンネル相続人の設定
	    PASSWORD      設立者パスワードの設定
	    DESC          チャンネルの説明の設定
	    URL           チャンネルと URL の紐付け
	    EMAIL         チャンネルと電子メールアドレスを紐付け
	    ENTRYMSG      チャンネルに入ったときの歓迎メッセージの設定
	    BANTYPE       チャンネルの禁止設定
	    MLOCK         チャンネルモードの ON/OFF をロック
	    KEEPTOPIC     チャンネルが使われていないときにトピックを保存
	    OPNOTICE      OP/DEOP コマンド使用時に notice を送る
	    PEACE         重要なコマンドの使用を制限
	    PRIVATE       LIST コマンドの結果にチャンネルを表示させない
	    RESTRICTED    チャンネルへのアクセスを制限
	    SECURE        %S の保安機能を有効化
	    SECUREOPS     チャンネルオペレータ状態の厳格な制御
	    SECUREFOUNDER チャンネル設立者状態の厳格な制御
	    SIGNKICK      KICK コマンドでのキックを署名する
	    TOPICLOCK     TOPIC コマンドだけがトピックを変更出来るようにする
	    XOP           ユーザ権限システムをトグル
	
	夫々のオプションに関するより詳しい情報は
	%R%S HELP SET option と入力。

CHAN_HELP_SET_FOUNDER
	構文: SET channel FOUNDER nick
	
	チャンネルの設立者を変更する。新しいニックネームは
	登録済みのものでなければいけません。

CHAN_HELP_SET_SUCCESSOR
	構文: SET channel SUCCESSOR nick
	
	チャンネル後継者を変更する。チャンネル登録が有効な状態で、
	設立者ニックネームが期限切れになったり、無効化された場合、
	チャンネルの後継者は新しいチャンネルの設立者になります。
	しかし、後継者がすでに多くの (%d) 登録済みチャンネルで設立者
	になっている場合で、どの後継者も設立者になれなかった場合、
	このチャンネル登録が無効化されます。新しいニックネームは
	登録済みのものでなくてはいけません。

CHAN_HELP_SET_PASSWORD
	構文: SET channel PASSWORD password
	
	チャンネルの設立者を認証するためのパスワードを設定する。

CHAN_HELP_SET_DESC
	構文: SET channel DESC description
	
	チャンネルの説明を設定する。この説明は
	LIST と INFO コマンドで見ることが出来る。

CHAN_HELP_SET_URL
	構文: SET channel URL [url]
	
	引数で指定された URL をチャンネルと紐付ける。この URL は
	INFO コマンドでチャンネルの情報をリクエストしたときに
	返される。パラメータが与えられなかった場合、このチャンネルに
	紐付けられた URL を削除する。

CHAN_HELP_SET_EMAIL
	構文: SET channel EMAIL [address]
	
	引数で指定された電子メールアドレスをチャンネルと紐付ける。
	このアドレスは INFO コマンドでチャンネルの情報を
	リクエストしたときに返される。パラメータが与えられなかった場合、
	このチャンネルに紐付けられた電子メールアドレスを削除する。

CHAN_HELP_SET_ENTRYMSG
	構文: SET channel ENTRYMSG [message]
	
	ユーザがチャンネルに入ったときの /notice 経由で送信する
	ユーザ宛メッセージを設定する。パラメータが無い場合、
	チャンネルに入ったときにメッセージを送らない。

CHAN_HELP_SET_BANTYPE
	構文: SET channel BANTYPE bantype
	
	バンタイプを設定する。サービスによってあなたのチャンネル
	から誰かを ban するときに、このタイプ分けが使われる。
	
	バンタイプは 0 から 3 の整数で表現される:
	
	0: *!user@host 型のバン指定
	1: *!*user@host 型のバン指定
	2: *!*@host 型のバン指定
	3: *!*user@*.domain 型のバン指定

CHAN_HELP_SET_KEEPTOPIC
	構文: SET channel KEEPTOPIC {ON | OFF}
	
	あるチャンネルのトピック保存オプションの有効と無効
	を切り替える。トピック保存が有効化されると、
	最後のユーザがこのチャンネルから退出した後も、
	トピックが %S によって記憶され、
	次にチャンネルが作られた時に自動的に設定される。

CHAN_HELP_SET_TOPICLOCK
	構文: SET channel TOPICLOCK {ON | OFF}
	
	あるチャンネルのトピックロックオプションの有効と無効
	を切り替える。トピックロック が有効化されると、%S は
	TOPIC コマンド以外ではチャンネルトピックの変更を
	禁止する。

CHAN_HELP_SET_MLOCK
	構文: SET channel MLOCK modes
	
	チャンネルのモードロックパラメータを設定する。
	%S は、あるチャンネルモードを常に ON にするか OFF にするか、
	自由に変更できるようにするかを定義することを可能にする。
	
	modes パラメータは /MODE コマンドのパラメータと同じもの
	が定義されています。つまり、+ に続けてモードパラメータを設定
	することで、ロックが ON になります。- に続けが場合は OFF
	になります。ただし、/MODE コマンドと異なり、SET MLOCK
	を使うことで以前にロックが ON にされたモードを OFF
	にすることが出来ます。
	
	警告: 下の 2 番目の例のようにモードロックキーを使った場合、
	このチャンネルに対して RESTRICTED オプションをセットするか、
	(HELP SET RESTRICTED を参照) 誰かがチャンネルに入って
	キーが変更されたかどうか確認してください。
	
	例:
	
	    SET #channel MLOCK +nt-iklps
	        モードの n と t を ON にして、i、k、l、p、s、を OFF にする。
		モード m は自由に変更できるようにする。
	
	    SET #channel MLOCK +knst-ilmp my-key
	        "my-key".
		モードの k、n、s、t を ON にして、i、l、m、p を OFF にする。
		また、チャンネルキーを "my-key" にする。
	
	    SET #channel MLOCK +
	        全てのモードロックを解除する。全てのチャンネルモードは
		自由に変更できる。

CHAN_HELP_SET_PEACE
	構文: SET channel PEACE {ON | OFF}
	
	あるチャンネルの peace オプションの有効と無効を切り替える。
	peace が有効化されると、%S コマンドを使って、ユーザはキック、
	バン又は自分より高位か同じレベルを持つユーザのチャンネル状態の
	削除をすることが出来なくなる。

CHAN_HELP_SET_PRIVATE
	構文: SET channel PRIVATE {ON | OFF}
	
	あるチャンネルの プライベートオプションの有効と無効を切り替える。
	プライベートが有効化されると、%R%S LIST コマンドの出力に
	このチャンネルは含まれなくなる。

CHAN_HELP_SET_RESTRICTED
	構文: SET channel RESTRICTED {ON | OFF}
	
	あるチャンネルのアクセス制限オプションの有効と無効を切り替える。
	アクセス制限が有効化されると、アクセスリストに含まれないユーザは
	チャンネルからキックされるかバンされる。

CHAN_HELP_SET_SECURE
	構文: SET channel SECURE {ON | OFF}
	
	あるチャンネルに対する %S の保安機能の有効と無効
	を切り替える。SECURE が有効化されると、
	アクセスリストによるコントロールと同様に、
	%s にニックネームが登録済みで、パスワードで IDENTIFY された
	ユーザだけがこのチャンネルにアクセスできるようになる。

CHAN_HELP_SET_SECUREOPS
	構文: SET channel SECUREOPS {ON | OFF}
	
	あるチャンネルに対するオペレータ保護オプションの有効と無効
	を切り替える。オペレータ保護が有効化されると、
	ユーザリストに含まれないユーザはチャンネル管理者になれなくなる。
	
CHAN_HELP_SET_SECUREFOUNDER
	構文: SET channel SECUREFOUNDER {ON | OFF}
	
	あるチャンネルに対する設立者保護オプションの有効と無効を切り替える。
	設立者保護が有効化されると、真のチャンネル設立者だけがチャンネルを
	未登録状態に変更できる。また、チャンネルパスワード、チャンネル設立者、
	チャンネル後継者、%S に IDENTIFY されていないユーザを変更できる。
	
CHAN_HELP_SET_SIGNKICK
	構文: SET channel SIGNKICK {ON | LEVEL | OFF}
	
	あるチャンネルに対する署名付きキックオプションの有効と無効
	を切り替える。SIGNKICK が有効化されると、
	%S KICK コマンドから発行されたキックはその理由の欄に
	コマンドを発行したユーザのニックネームの署名を含むようになる。
	
	LEVEL が使用された場合、SIGNKICK レベルよりも高位又は同位
	のレベルを持つユーザからのキックには署名がなされない。
	より詳しい情報は %R%S HELP LEVELS を入力してください。

CHAN_HELP_SET_XOP
	構文: SET channel XOP {ON | OFF}
	
	あるチャンネルに対する xOP リストシステムの有効と無効
	を切り替える。XOP が有効化されると、
	ユーザにチャンネルでの権限を与えるには AOP/SOP/VOP
	を使うか、ACCESS コマンドを使うことになる。
	
	技術的な注意: アクセススリストから xOP リスト知る手無に変更
	するときには、あなたのレベル定義とユーザレベルも変更されなければ
	いけません。そうでなければ、アクセスリストシステムに戻った際に
	同じレベルを使えなくなります。
	
	xOP リストを変更した後に、ユーザが許可側の xOP リストにいるかどうか
	チェックしてください。なぜなら、思い込みはいつも
	完璧であるとは限らないからです。実際、LEVEL コマンドを
	使ってレベルの定義を変更する場合には、xOP リストを使うこと
	は推奨されません。
	
	xOP リストシステムからアクセスリストシステムに変更
	する場合には問題は起こりません。

CHAN_HELP_SET_OPNOTICE
	構文: SET channel OPNOTICE {ON | OFF}
	
	あるチャンネルに対する op-notice オプションの有効と無効
	を切り替える。op-notice が有効化されると、%S は
	OP 又は DEOP がこのチャンネルにいるユーザに対して
	使用されたときに notice をチャンネルに送信する。

CHAN_HELP_AOP
	構文: AOP channel ADD nick
	      AOP channel DEL {nick | entry-num | list}
	      AOP channel LIST [mask | list]
	      AOP channel CLEAR
	
	あるチャンネルに対する AOP (自動 OP) リストを編集
	する。AOP リストはあなたのチャンネルで自動的にオペレータ
	になることの出来るユーザのリストです。バン解除と招待すること
	も出来ます。もし必要であればユーザに対する歓迎メッセージを
	設定することも出来ます。
	
	AOP ADD コマンドは与えられたニックネームを AOP リスト
	に追加する。
	
	AOP DEL コマンドは与えられたニックネームを AOP リスト
	から削除する。もし、リストのエントリ番号が与えられた場合、
	このエントリを削除する (下の LIST に関する例を見よ)。
	
	AOP LIST コマンドは AOP リストを表示する。もしワイルドカード
	マスクが与えられた場合、マスクにマッチするエントリだけが
	表示される。もしリストのエントリ番号が与えられた場合、
	このエントリ番号だけが表示される。例えば、

	   AOP #channel LIST 2-5,7-9
	      2 から 5、7 から 9 番目の AOP
	      リストエントリを表示する。
	      
	AOP CLEAR コマンドは AOP リストの全てのエントリを
	削除する。
	
	AOP ADD と AOP DEL コマンドは SOP かそれより高位の
	権限を持つものだけに使用が制限されています。これに対して、
	AOP CLEAR コマンドは、チャンネル設立者だけが使えます。
	しかし、AOP リストに登録されているユーザであれば誰でも
	AOP LIST コマンドを使うことが出来ます。
	
	このコマンドをあなたのチャンネルで使用できないように出来ます。
	その場合は、アクセスリストを使わなければいけません。
	%R%S HELP ACCESS と入力することで、アクセスリストに関する
	より詳しい情報が見れます。アクセスリストと xOP リストを
	切り替える方法に関する詳細は %R%S HELP SET XOP と入力してください。

CHAN_HELP_HOP
	構文: HOP channel ADD nick
	      HOP channel DEL {nick | entry-num | list}
	      HOP channel LIST [mask | list]
	      HOP channel CLEAR
	
	あるチャンネルに対する HOP (HalfOP) リストを編集
	する。チャンネルの HOP リストに登録されているユーザは、自動で
	halfop されます。
	
	HOP ADD コマンドは与えられたニックネームを HOP リスト
	に追加する。
	
	HOP DEL コマンドは与えられたニックネームを HOP リスト
	から削除する。もし、リストのエントリ番号が与えられた場合、
	このエントリを削除する (下の LIST に関する例を見よ)。
	
	HOP LIST コマンドは HOP リストを表示する。もしワイルドカード
	マスクが与えられた場合、マスクにマッチするエントリだけが
	表示される。もしリストのエントリ番号が与えられた場合、
	このエントリ番号だけが表示される。例えば、

	   HOP #channel LIST 2-5,7-9
	      2 から 5、7 から 9 番目の HOP
	      リストエントリを表示する。
	      
	HOP CLEAR コマンドは HOP リストの全てのエントリを
	削除する。
	
	HOP ADD、 HOP DEL と HOP LIST コマンドは AOP か
	それより高位の権限を持つものだけに使用が制限されています。
	これに対して、HOP CLEAR コマンドは、チャンネル設立者
	だけが使えます。
	
	このコマンドをあなたのチャンネルで使用できないように出来ます。
	その場合は、アクセスリストを使わなければいけません。
	%R%S HELP ACCESS と入力することで、アクセスリストに関する
	より詳しい情報が見れます。アクセスリストと xOP リストを
	切り替える方法に関する詳細は %R%S HELP SET XOP と入力してください。

CHAN_HELP_SOP
	構文: SOP channel ADD nick
	      SOP channel DEL {nick | entry-num | list}
	      SOP channel LIST [mask | list]
	      SOP channel CLEAR
	
	あるチャンネルに対する SOP (SuperOP) リストを編集
	する。チャンネルの SOP リストに登録されているユーザは、AOP
	リストで得られる全ての権限が与えられ、自動キックと
	禁止ワードリストの可否、チャンネルメモの送信と受信などを
	行えます。
	
	SOP ADD コマンドは与えられたニックネームを SOP リスト
	に追加する。
	
	SOP DEL コマンドは与えられたニックネームを SOP リスト
	から削除する。もし、リストのエントリ番号が与えられた場合、
	このエントリを削除する。(下の LIST に関する例を見よ)
	
	SOP LIST コマンドは SOP リストを表示する。もしワイルドカード
	マスクが与えられた場合、マスクにマッチするエントリだけが
	表示される。もしリストのエントリ番号が与えられた場合、
	このエントリ番号だけが表示される。例えば、

	   SOP #channel LIST 2-5,7-9
	      2 から 5、7 から 9 番目の HOP リストエントリを表示する。
	      
	SOP CLEAR コマンドは SOP リストの全てのエントリを
	削除する。
	
	SOP ADD、 SOP DEL と SOP CLEAR コマンドは
	チャンネル設立者だけに使用が制限されています。
	これに対して、HOP LIST コマンドは、AOP リストに含まれる
	ユーザであれば誰でも使えます。
	
	このコマンドをあなたのチャンネルで使用できないように出来ます。
	その場合は、アクセスリストを使わなければいけません。
	%R%S HELP ACCESS と入力することで、アクセスリストに関する
	より詳しい情報が見れます。アクセスリストと xOP リストを
	切り替える方法に関する詳細は %R%S HELP SET XOP
	と入力してください。

CHAN_HELP_VOP
	構文: VOP channel ADD nick
	      VOP channel DEL {nick | entry-num | list}
	      VOP channel LIST [mask | list]
	      VOP channel CLEAR
	
	あるチャンネルに対する VOP (VOicePeople) リストを編集
	する。チャンネルの VOP リストに登録されているユーザは、自動的に
	発言権を与えられ、もし発言権を与えられていなくても発言出来ます。
	
	VOP ADD コマンドは与えられたニックネームを VOP リスト
	に追加する。
	
	VOP DEL コマンドは与えられたニックネームを VOP リスト
	から削除する。もし、リストのエントリ番号が与えられた場合、
	このエントリを削除する (下の LIST に関する例を見よ)。
	
	VOP LIST コマンドは VOP リストを表示する。もしワイルドカード
	マスクが与えられた場合、マスクにマッチするエントリだけが
	表示される。もしリストのエントリ番号が与えられた場合、
	このエントリ番号だけが表示される。例えば、

	   VOP #channel LIST 2-5,7-9
	      2 から 5、7 から 9 番目の VOP
	      リストエントリを表示する。
	      
	VOP CLEAR コマンドは VOP リストの全てのエントリを
	削除する。
	
	VOP ADD、 VOP DEL と VOP LIST コマンドは AOP か
	それより高位の権限を持つものだけに使用が制限されています。
	これに対して、HOP CLEAR コマンドは、チャンネル設立者だけが使えます。
	
	このコマンドをあなたのチャンネルで使用できないように出来ます。
	その場合は、アクセスリストを使わなければいけません。
	%R%S HELP ACCESS と入力することで、アクセスリストに関する
	より詳しい情報が見れます。アクセスリストと xOP リストを
	切り替える方法に関する詳細は %R%S HELP SET XOP と入力してください。

CHAN_HELP_ACCESS
	構文: ACCESS channel ADD nick level
	      ACCESS channel DEL {nick | entry-num | list}
	      ACCESS channel LIST [mask | list]
	      ACCESS channel CLEAR
	
	チャンネルのアクセスリストを編集する。アクセスリストは
	チャンネル管理者になれるユーザ、又はこのチャンネルの %S コマンド
	にアクセスできるユーザを指定します。異なるユーザレベル
	のユーザには異なる権限を与えます。より詳しい情報は、
	%R%S HELP ACCESS LEVELS を参照してください。アクセスリスト
	に載っていない全てのニックネームのユーザレベルは 0 です。
	
	ACCESS ADD コマンドは与えられたニックネームとそのレベル
	をアクセスリストに追加する。もしニックネームがすでにアクセス
	リストに登録済みであれば、アクセスレベルが変更される。
	コマンドを発行したユーザは自分よりも高位の level を
	与えることは出来ない。また、nick がすでにアクセスリスト
	に登録済みの場合、このニックネームの登録済みのアクセスレベル
	はコマンドで指定されたものよりも低いものでなければいけない。
	
	ACCESS DEL コマンドは与えられたニックネームを VOP リスト
	から削除する。もし、リストのエントリ番号が与えられた場合、
	このエントリを削除する (下の LIST に関する例を見よ)。
	
	ACCESS LIST コマンドは VOP リストを表示する。もしワイルドカード
	マスクが与えられた場合、マスクにマッチするエントリだけが
	表示される。もしリストのエントリ番号が与えられた場合、
	このエントリ番号だけが表示される。例えば:

	   ACCESS #channel LIST 2-5,7-9
	      2 から 5、7 から 9 番目のアクセスリスト
	      エントリを表示する。
	      
	ACCESS CLEAR コマンドはアクセスリストの全てのエントリを
	削除する。

CHAN_HELP_ACCESS_LEVELS
	ユーザアクセスレベル
	
	初期状態では、以下のアクセスレベルが定義されています。
	
	   設立者    %S の機能にフルアクセスできる。チャンネルに
	                     入った時に自動オペレータ化される。
	                     注意することは、1 人だけが設立者権限
	                     (この権限は ACCESS コマンドで誰かに
	                     貸与できない。)
	        10   AKICK コマンドにアクセスできる。自動オペレータ権付加。
	         5   自動オペレータ権付加。
	         3   自動発言権付加。
	         0   特別な権限を持たない。別のオペレータからオペレータ
	                     権限を貰う (オペレータ保護が無効の場合)。
	        <0   オペレータになれない。
	
	これらのレベルは変更可能で、LEVELS コマンドを使って
	新しいレベルを加えることが出来る。より詳しい情報には
	%R%S HELP LEVELS と入力。

CHAN_HELP_AKICK
	構文: AKICK channel ADD {nick | mask} [reason]
	      AKICK channel STICK mask
	      AKICK channel UNSTICK mask
	      AKICK channel DEL {nick | mask | entry-num | list}
	      AKICK channel LIST [mask | entry-num | list]
	      AKICK channel VIEW [mask | entry-num | list]
	      AKICK channel ENFORCE
	      AKICK channel CLEAR
	
	あるチャンネルの自動キックリストを編集する。
	自動キックリストに登録されているユーザがチャンネルに
	参加しようとした場合には、%S はこのユーザをチャンネルから
	ban して、その後にキックする。
	
	AKICK ADD コマンドは与えられたニックネームかユーザマスク
	を自動キックリストに追加する。もし reason が与えられた
	場合は、この理由はユーザがキックされるときに使用される。
	もし与えなければ、初期値である "You have been
	banned from the channel" が使われる。
	登録済みニックネームを自動キックする場合は、この nickserv
	アカウントがマスクの代わりに akick リストに加えられ、
	このアカウントに関するニックネームグループに含まれる全ての
	ユーザが自動キックリストに加えられる。
	
	AKICK STICK コマンドはあるチャンネルで与えられたマスクに
	該当するユーザを ban する。もし誰かがこの ban を削除しようとした
	場合には、%S は自動的にこれを回復する。この機能を登録済み
	ニックネームに対して使うことはできない。
	
	AKICK UNSTICK コマンドは AKICK STICK の効果を
	キャンセルする。そのため、このチャンネルに関する ban を
	キャンセルすることが出来る。
	
	AKICK DEL コマンドは与えられたニックネームかマスクを
	自動キックリストから削除する。もしこれらの引数が与えられ
	なければ、自動キックリストに含まれる全ての ban を削除する。
	このようなことは通常は 1 つずつ行われるべきである。
	
	AKICK LIST コマンドは自動キックリストを表示するか、
	与えられたマスクに一致する自動キックリストエントリの
	一部を表示する。
	
	AKICK VIEW コマンドは AKICK LIST のより詳細な
	バージョンである。
	
	AKICK ENFORCE コマンドは AKICK マスクにマッチする
	ユーザを削除することで、現在の自動キックリストを %S
	に適応させる。
	
	AKICK CLEAR コマンドは自動キックリストに含まれる
	全てのエントリを削除する。

CHAN_HELP_LEVELS
	構文: LEVELS channel SET type level
	      LEVELS channel {DIS | DISABLE} type
	      LEVELS channel LIST
	      LEVELS channel RESET
	
	LEVELS コマンドはチャンネル内でアクセスレベルを設定することで
	詳細なアクセスコントロールを可能にする。このコマンドを使うことで
	%S の機能の殆どで使用されるアクセスレベルを定義できる。
	(SET FOUNDER と SET PASSWORD コマンドとこのコマンドは、
	チャンネル設立者だけに使用が制限されている。)
	
	LEVELS SET コマンドはアクセスレベルの機能を変更するか、
	グループの機能を変更するときに使う。LEVELS DISABLE
	(又は DIS) は自動配布機能の無効化やある機能に対する
	チャンネル設立者以外のユーザのアクセス禁止をおこなう。
	LEVELS LIST はそれぞれの機能やグループがどのような
	レベルに許可されているかのリストを表示する。
	LEVELS RESET はアクセスレベルの設定を新しく作成された
	チャンネルに適用されれるデフォルトレベルに戻す
	(HELP ACCESS LEVELS を参照)。
	
	全ての機能のリストとどのようなレベルが設定できるかに関する
	詳細は HELP LEVELS DESC を参照。

CHAN_HELP_LEVELS_DESC
	以下の機能が認識できる。AUTODEOP と NOJOIN レベルは
	最大のレベルで、それ以外は最低のレベルである。

CHAN_HELP_LEVELS_DESC_FORMAT
	    %-*s  %s

CHAN_HELP_INFO
	構文: INFO channel [ALL]
	
	登録済みチャンネルに関する情報を表示する。
	情報には、チャンネル設立者、登録日時、最終使用日時、
	説明、もしあるならロックモード、が含まれる。ALL
	が与えられた場合、歓迎メッセージ、後継者の情報も
	含まれる。
	
	初期状態では、ALL オプションはチャンネル設立者以外は
	使えないようになっている。

CHAN_HELP_LIST
	構文: LIST pattern
	
	与えられたパターンにマッチする全ての登録済みチャンネルを表示
	する (PRIVATE オプションが有効になっているチャンネルは
	リストされません)。# から始まるとそれは範囲を意味します。
	チャンネル名に # をつける必要はありません。

CHAN_HELP_OP
	構文: OP [#channel [nick]]
	
	あるチャンネルのニックネームにオペレータ権限を配布します。
	ニックネームが与えられなかった場合には、自分にオペレータ
	権限を配布します。チャンネルとニックネームが与えられ
	なかった場合は、自分が参加している全てのチャンネルで
	自分にオペレータ権限が配布されます。ただし、オペレータ
	権限を貰う権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_DEOP
	構文: DEOP [#channel [nick]]
	
	与えられたチャンネルのニックネームからオペレータ権限を剥奪する。
	もしニックネームが与えられなかった場合には、自分から
	オペレータ権限を剥奪する。チャンネルとニックネームが
	与えられなかった場合には、自分が参加している全てのチャンネルで
	自分からオペレータ権限を剥奪します。ただし、オペレータ
	権限を剥奪できる権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。
	
CHAN_HELP_VOICE
	構文: VOICE [#channel [nick]]
	
	チャンネルの与えられたニックネームに発言権を与える。
	ニックネームが与えられなかった場合、自分に発言権を
	与える。チャンネル名とニックネームが与えられなかった場合、
	参加している全てのチャンネルの自分に発言権を与える。
	ただし、発言権を与えられる権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザか、VOP でレベル 3 以上のアクセスレベル
	を持っているユーザだけに使用が制限されています。

CHAN_HELP_DEVOICE
	構文: DEVOICE [#channel [nick]]
	
	チャンネルの与えられたニックネームから発言権を剥奪する。
	ニックネームが与えられなかった場合、自分から発言権を
	剥奪する。チャンネル名とニックネームが与えられなかった場合、
	参加している全てのチャンネルの自分から発言権を剥奪する。
	ただし、発言権を剥奪できる権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザか、VOP でレベル 3 以上のアクセスレベル
	を持っているユーザだけに使用が制限されています。
	
CHAN_HELP_HALFOP
	構文: HALFOP [#channel [nick]]
	
	与えられたチャンネルにいるニックネームに Halfops 権を付与する。
	ニックネームが与えられなかった場合、自分自身に halfops 権
	を付与する。チャンネル名とニックネームが与えられなかった場合、
	参加している全てのチャンネルの自分に halfops 権を付与する。
	ただし、halfops 権を付与できる権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザか、HOP でレベル 4 以上のアクセスレベル
	を持っているユーザだけに使用が制限されています。

CHAN_HELP_DEHALFOP
	構文: DEHALFOP [#channel [nick]]
	
	与えられたチャンネルにいるニックネームから Halfops 権を剥奪する。
	ニックネームが与えられなかった場合、自分自身から halfops 権
	を剥奪する。チャンネル名とニックネームが与えられなかった場合、
	参加している全てのチャンネルの自分から halfops 権を剥奪する。
	ただし、halfops 権を剥奪できる権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザか、HOP でレベル 4 以上のアクセスレベル
	を持っているユーザだけに使用が制限されています。
	
CHAN_HELP_PROTECT
	構文: PROTECT [#channel [nick]]
	
	与えられたチャンネルにいるニックネームを保護する。ニックネームが
	与えられなかった場合、自分自身を保護する。チャンネル名とニックネームが
	与えられなかった場合、参加している全てのチャンネルの自分を保護する。
	ただし、保護できる権利がある場合に限られます。
	
	初期状態では、このコマンドは設立者か、SOP でレベル 10 以上の
	アクセスレベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_DEPROTECT
	構文: DEPROTECT [#channel [nick]]
	
	与えられたチャンネルにいるニックネームを保護しない。ニックネームが
	与えられなかった場合、自分自身を保護しない。チャンネル名とニックネームが
	与えられなかった場合、参加している全てのチャンネルの自分を保護しない。
	ただし、保護しない権利がある場合に限られます。
	
	初期状態では、このコマンドは設立者か、SOP でレベル 10 以上の
	アクセスレベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_OWNER
	構文: OWNER [#channel]
	
	与えられた channel の所有権を自分に付加する。チャンネル名が
	与えられなかった場合、参加している全てのチャンネルの自分自身に
	所有権を付加する。ただし、所有権を付加できる権利がある場合に限られます。
	
	このコマンドはチャンネルにいる設立者だけに使用が制限されています。

CHAN_HELP_DEOWNER
	構文: DEOWNER [#channel]
	
	与えられた channel の所有権を自分から剥奪する。チャンネル名が
	与えられなかった場合、参加している全てのチャンネルの自分自身から
	所有権を剥奪する。ただし、所有権を剥奪できる権利がある場合に限られます。
	
	このコマンドはチャンネルにいる設立者だけに使用が制限されています。

CHAN_HELP_INVITE
	構文: INVITE channel
	
	%S は与えられたチャンネルにあなたを招待します。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_UNBAN
	構文: UNBAN channel
	
	%S は与えられたチャンネルにあなたが入ることを妨げる
	全ての ban を削除します。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。
	
CHAN_HELP_KICK
	構文: KICK [#channel [nick [reason]]]
	
	与えられたチャンネルにいるニックネームをキックします。ニックネームが
	与えられなかった場合、自分自身をキックします。チャンネル名とニックネームが
	与えられなかった場合、参加している全てのチャンネルの自分をキックします。
	ただし、キックする権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_BAN
	構文: BAN [#channel [nick [reason]]]
	
	与えられたチャンネルにいるニックネームを ban します。ニックネームが
	与えられなかった場合、自分自身を ban します。チャンネル名とニックネームが
	与えられなかった場合、参加している全てのチャンネルの自分を ban します。
	ただし、ban する権利がある場合に限られます。
	
	初期状態では、このコマンドは AOP でレベル 5 以上のアクセス
	レベルを持っているユーザだけに使用が制限されています。

CHAN_HELP_TOPIC
	構文: TOPIC channel [topic]
	
	%S にチャンネルトピックを与えられたものに変更させる。
	topic が与えられなかった場合、空のトピックになります。
	このコマンドは SET TOPICLOCK と組み合わせて使うと
	便利です。より詳しい情報は %R%S HELP SET TOPICLOCK
	を参照してください。

	このコマンドはチャンネルにいる設立者だけに使用が制限
	されています。

CHAN_HELP_CLEAR
	構文: CLEAR channel what
	
	%S にチャンネルの設定を初期状態に戻させる。what は以下の
	1 つである。
	
	     MODES    全てのチャンネルモードをリセットする (例:
	                  i、k、l、m、n、p、s、t をクリアする)。
	     BANS     チャンネルの ban エントリを全てのクリアする。
	     EXCEPTS  チャンネルの例外エントリを全てクリアする。
	     INVITES  チャンネルの招待エントリを全てクリアする。
	     OPS      チャンネルオペレータ (+o) 全員から
	                  オペレータ権を剥奪する。
	     HOPS     channel-halfoperator (+h) 全員からこの権利を剥奪する。
	                  ただしサポートされている場合に限る。
	     VOICES   発言権 (+v) を持つユーザ全員から
	                  発言権を剥奪する。
	     USERS    チャンネルにいるユーザ全員をキックする。
	
	このコマンドはチャンネルにいる設立者だけに使用が制限
	されています。
	
CHAN_HELP_GETKEY
	構文: GETKEY channel
	
	与えられたチャンネルのキーを返す。このコマンドは、
	ボットかスクリプトによって使用されることを目的としています。
	そのため、出力のフォーマットは以下です。
	
	KEY <channel> <key>
	
	キーがセットされていない場合、キーが "NO KEY" として返されます。
	
CHAN_HELP_SENDPASS
	構文: SENDPASS channel
	
	与えられたチャンネルのパスワードを設立者のニックネームレコード
	に登録された電子メールアドレス宛に送信します。このコマンドは
	パスワードを紛失したときに役に立ちます。
	
	このコマンドを使用できるのは、IRC オペレータだけに制限されています。
	
	このコマンドは暗号化が有効な場合には使用できません。

CHAN_SERVADMIN_HELP
	 
	サービス管理者はパスワード認証無しに、全てのチャンネルを
	無効化、アクセスチェック、AKICK、レベル設定できます。
	
CHAN_SERVADMIN_HELP_LOGOUT
	構文: LOGOUT channel [nickname]
	
	このコマンドは与えられたニックネームをログアウトさせます。
	つまり、ログアウトさせられたユーザが再度アクセスするためには
	認証が必要になります。
	
	もしあなたがチャンネル設立者だった場合は、自分のチャンネル
	にいるユーザであれば誰でも強制ログアウトさせることが出来ます。
	そうでない場合は、強制ログアウトさせることが出来るのは自分だけです。
	
	もしあなたがサービス管理者だった場合は、チャンネル設立者
	の権限を持っていなくてもどのチャンネルにいるどのユーザでも
	強制ログアウトさせることが出来ます。また、ニックネーム
	パラメータを省略することで、チャンネルにいる認証済み
	ユーザを全員強制ログアウトさせることが出来ます。

CHAN_SERVADMIN_HELP_DROP
	構文: DROP channel
	
	チャンネルの登録を無効化する。認証済みでないユーザが
	これをするにはサービス管理者である必要があります。

CHAN_SERVADMIN_HELP_SET
	
	サービス管理者は NOEXPIRE オプションをセットすることが出来ます。
	これがセットされたチャンネルは登録を抹消できなくなります。
	加えて、サービス管理者はチャンネルパスワードによる
	認証無しにチャンネルオプションを設定することが出来ます。

CHAN_SERVADMIN_HELP_SET_NOEXPIRE
	構文: SET channel NOEXPIRE {ON | OFF}
	
	与えられたチャンネルの登録を抹消するかどうか設定する。
	ON の場合はチャンネル登録を抹消できなくなります。

CHAN_SERVADMIN_HELP_INFO
	
	サービス管理者はどんなチャンネルに対しても
	ALL パラメータを使うことが出来ます。

CHAN_SERVADMIN_HELP_LIST
	構文: LIST pattern [FORBIDDEN] [SUSPENDED] [NOEXPIRE]
	
	与えられたパターンにマッチする全ての登録済みチャンネルをリスト
	する。PRIVATE オプションが有効になっているチャンネルは
	サービス管理者に対してのみリストされます。NOEXPIRE オプション
	が有効になっているチャンネルは ! がチャンネル名に付加され、
	サービス管理者に対してのみリストされます。
	
	FORBIDDEN、SUSPENDED、NOEXPIRE オプションが与えられた場合、
	それぞれのオプションが有効化されているチャンネルだけが表示されます。
	複数のオプションが与えられた場合、それぞれにマッチする
	チャンネルが表示されます。これらのオプションが使えるのは
	サービス管理者だけに制限されています。

CHAN_SERVADMIN_HELP_GETPASS
	構文: GETPASS channel
	
	与えられたチャンネルのパスワードを返します。注意する点
	は、このコマンドが使用された場合、コマンドを発行したユーザと
	チャンネル名が記録され、WALLOPS か GLOBOPS として送信
	されます。

CHAN_SERVADMIN_HELP_FORBID
	構文: FORBID channel [reason]
	
	与えられたチャンネル名を登録することと、これを使用することを
	禁止する。このチャンネルを無効化することでキャンセルすることが出来る。
	
	一部のネットワークでは理由を書くことが必須です。

CHAN_SERVADMIN_HELP_SUSPEND
	構文: SUSPEND channel [reason]
	
	与えられたチャンネル名を登録することと、これを使用することを
	禁止する。UNSUSPEND コマンドを使用することで、チャンネルデータと
	設定を保存したままでキャンセルすることが出来る。
	
	一部のネットワークでは理由を書くことが必須です。

CHAN_SERVADMIN_HELP_UNSUSPEND
	構文: UNSUSPEND channel
	
	一時停止状態のチャンネルを破棄する。ただし、全てのデータと
	設定は保存される。

CHAN_SERVADMIN_HELP_STATUS
	構文: STATUS channel nickname
	
	与えられたチャンネルにいるニックネームの現在の
	アクセスレベルを返す。応答は以下のフォーマットに従う。
	
	    STATUS channel nickname access-level
	
	エラーが起きた場合、以下のフォーマットが返される。
	
	    STATUS ERROR error-message

###########################################################################
#
# MemoServ help messages
#
###########################################################################

MEMO_HELP_CMD_SEND
	    SEND   ニックネームかチャンネル宛てにメモを送信
MEMO_HELP_CMD_CANCEL
	    CANCEL 最新のメモの送信をキャンセル
MEMO_HELP_CMD_LIST
	    LIST   あなた宛のメモを表示
MEMO_HELP_CMD_READ
	    READ   メモを読む
MEMO_HELP_CMD_DEL
	    DEL    メモを削除する
MEMO_HELP_CMD_SET
	    SET    メモに関するオプションの設定
MEMO_HELP_CMD_INFO
	    INFO   あなた宛のメモに関する情報を表示
MEMO_HELP_CMD_RSEND
	    RSEND  メモを送信して、開封確認を要求する
MEMO_HELP_CMD_CHECK
	    CHECK  ニックネーム宛ての最新のメモが読まれたかチェックする
MEMO_HELP_CMD_SENDALL
	    SENDALL  登録済みのユーザ全員にメモを送信
MEMO_HELP_CMD_STAFF
	    STAFF  サーバオペレータと管理者にメモを送信

MEMO_HELP_HEADER
	IRC ユーザは %S を使って他の IRC ユーザやチャンネル
	に短いメッセージを送信できます。ユーザ宛てのメッセージは、
	その IRC ユーザがオンラインでなくても送信できます。
	メモを送信するためには、あなたのニックネームと宛先の
	ニックネーム、又はチャンネル名が登録済みでなければいけません。

	%S 機能に関するコマンド:


MEMO_HELP_ADMIN
	使われていません。
MEMO_HELP_OPER
	使われていません。

MEMO_HELP_FOOTER
	
	%R%S HELP command と入力すれば、上に挙げたコマンドの
	ヘルプを見ることができます。

	(*) 初期状態では、チャンネルに参加中で権限レベル 10 以上
	    のユーザのみがチャンネルメモを読むことができます。
	    このレベルは %s LEVELS コマンドで変更できます。

MEMO_HELP_SEND
	構文: SEND {nick | channel} memo-text
	
	ニックネーム nick か channel 宛てに内容 memo-text
	のメモを送信します。ニックネーム宛てにメモを送信する場合は、
	受取人に新着メモを通知します。宛先のニックネームやチャンネル名は
	登録済みのものでなければいけません。
	
MEMO_HELP_CANCEL
	構文: CANCEL {nick | channel}
	
	指定されたニックネームかチャンネル宛てに送信した最新のメモを
	キャンセルし、このコマンドを実行後はメモが読まれないようにします。

MEMO_HELP_LIST
	構文: LIST [channel] [list | NEW]
	
	あなたが現在受け取っているメモを表示します。NEW が指定された
	場合、未読のメモだけを表示します。未読のメモにはメモ番号の左に"*"
	がついています。また以下のように、番号を指定して表示させること
	もできます。

	   LIST 2-5,7-9
	      2 から 5 番と 7 から 8 番のメモを表示します。

MEMO_HELP_READ
	構文: READ [channel] {num | list | LAST | NEW}
	
	指定されたメモの内容を表示します。LAST が指定された場合は、
	最新のメモを表示します。NEW が指定された場合は、新着メモを
	全て表示します。これら以外の場合、num で指定された
	メモを表示します。また、以下のように、一度に複数の番号を
	指定することもできます。
	
	   READ 2-5,7-9
	      2 から 5 番と 7 から 8 番のメモを表示します。

MEMO_HELP_DEL
	構文: DEL [channel] {num | list | LAST | ALL}
	
	指定されたメモ削除します。削除するメモ番号を 1 個づつ指定
	することもできますが、以下の 2 番目の例のように、複数の
	メモ番号を指定したり、範囲で番号を指定することもできます。
	
	LAST が指定された場合は、最新のメモが削除されます。
	ALL が指定された場合は、全てのメモが削除されます。
	
	例:
	
	   DEL 1
	      最初のメモを削除します。
	
	   DEL 2-5,7-9
	      2 から 5 番と 7 から 8 番のメモを削除します。

MEMO_HELP_SET
	構文: SET option parameters

	メモ機能に関するオプションを設定します。 option に指定できるのは以下:
	
	    NOTIFY      新着メモを通知するタイミングを変更します。
			    引数ではニックネームを指定します。
	    LIMIT       あなたが受け取るメモの上限数を設定します。
	
	それぞれのオプションについて、%R%S HELP SET option
	と入力すれば、より詳しい情報を見ることができます。

MEMO_HELP_SET_NOTIFY
	構文: SET NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}

	新着メモを通知するタイミングを変更します:
	
	    ON      ログオン時、/AWAY の解除時、メモが到着した時に
	               新着メモを通知します。
	    LOGON   ログオン時と /AWAY の解除時に 新着メモを
	               通知します。
	    NEW     メモが到着した時に新着メモを通知します。
	    MAIL    上の通知設定に加えて、メールでも新着メモを
	               通知します。
	    NOMAIL  メールでの新着メモの通知を止めます。
	    OFF     新着メモの通知を止めます。
	
	ON は LOGON と NEW を併せたものに相当します。

MEMO_HELP_SET_LIMIT
	構文: SET LIMIT [channel] limit
	
	あなた (又は指定されたチャンネル) のメモ保存量の上限値を
	設定します。0 を指定した場合は、あなた宛のメモを誰も送信
	できなくなります。ただし、%d よりも大きな数値を指定する
	ことはできません。

MEMO_HELP_INFO
	構文: INFO [channel]
	
	受け取ったメモの統計情報 (未読数、上限)
	を表示します。引数を指定すると、これらの統計情報を
	チャンネル宛てに送られたメモについて表示します。

MEMO_SERVADMIN_HELP_SET_LIMIT
	構文: SET LIMIT [user | channel] {limit | NONE} [HARD]
	
	ユーザかチャンネルのメモ保存量の上限値を設定します。
	0 を指定した場合は、ユーザがメモを受け取ることを禁止します。
	NONE を指定した場合は、ユーザのメモ保存量の上限を
	無くします。ニックネームかチャンネル名を指定しなかった場合は、
	あなた自身のメモ保存量の上限値を設定します。
	
	HARD を追加すると、ユーザがメモ保存量の上限値を変更
	できなくします。HARD を指定した後でも、HARD を
	外して同じコマンドを入力すれば、ユーザが上限値を変更
	できるようにします。

	SET LIMIT コマンドを使えるのはサービス管理者だけです。
	他のユーザは自分自身かチャンネルのメモ保存量の上限値を変える
	ことができます。ただし、チャンネルのメモ保存量の上限値を変える
	には、相応の権限が必要です。また、サービス管理者以外のユーザは
	メモ保存量の上限値を NONE にすること、%d 以上の上限値を設定する
	こと、HARD を指定することはできません。

MEMO_SERVADMIN_HELP_INFO
	構文: INFO [nick | channel]

	引数が指定されなかった場合、あなたが受け取っている
	メモの統計情報 (未読数、総数、上限値) を表示します。
	
	チャンネルを指定すると、同じ統計情報を指定したチャンネル
	について表示します。
	
	ニックネームを指定すると、同じ統計情報を指定した
	ニックネームについて表示します。ニックネームの指定は
	サービス管理者だけが行えます。

MEMO_HELP_STAFF
	構文: STAFF memo-text

	すべてのサービススタッフに memo-text の内容でメモを
	送信します。注意: サーバオペレータでかつサービス管理者
	でもあるユーザは、この内容のメモを 2 回受け取ることになります。
	つまり、複数のサービススタッフリストに登録されているユーザは
	同じ内容のメモを複数回受け取ることになります。

MEMO_HELP_SENDALL
	構文: SENDALL memo-text

	登録済みユーザ全員に内容 memo-text でメモを送信します。
	
MEMO_HELP_RSEND
	構文: RSEND {nick | channel} memo-text
	
	nick か channel 宛てに内容 memo-text でメモを送信します。
	宛先がニックネームの場合は、受取人は新規メモの到着を通知されます。
	宛先のニックネーム/チャンネル名は登録済みの者でなければいけません。
	受取人がメモを読むと、自動的にメモが開封されたことの通知が
	送信者宛てに送られます。

MEMO_HELP_CHECK
	構文: CHECK nick
	
	nick 宛てに送信した最新のメモが開封されたか否かを確認します。
	この機能はニックネームに対してのみ有効で、チャンネルに対しては無効です。

###########################################################################
#
# OperServ help messages
#
###########################################################################
OPER_HELP_CMD_GLOBAL
	    GLOBAL      メッセージをユーザ全員に送ります。
OPER_HELP_CMD_STATS
	    STATS       サービスとネットワークのの状態を表示します。
OPER_HELP_CMD_OPER
	    OPER        サービスオペレータのリストを変更します。
OPER_HELP_CMD_ADMIN
	    ADMIN       サービス管理者のリストほ変更します。
OPER_HELP_CMD_STAFF
	    STAFF       サービススタッフとオンラインか否かを表示します。
OPER_HELP_CMD_MODE
	    MODE        チャンネルモードを変更します。
OPER_HELP_CMD_KICK
	    KICK        ユーザをチャンネルからキックします。
OPER_HELP_CMD_CLEARMODES
	    CLEARMODES  チャンネルのモードを初期化します。
OPER_HELP_CMD_KILLCLONES
	    KILLCLONES  あるホストからの接続を全て KILL します。
OPER_HELP_CMD_AKILL
	    AKILL       AKILL リストを編集します。
OPER_HELP_CMD_SGLINE
	    SGLINE      SGLINE リストを編集します。
OPER_HELP_CMD_SQLINE
	    SQLINE      SQLINE リストを編集します。
OPER_HELP_CMD_SZLINE
	    SZLINE      SZLINE リストを編集します。
OPER_HELP_CMD_CHANLIST
	    CHANLIST    全てのチャンネルレコードを表示します。
OPER_HELP_CMD_USERLIST
	    USERLIST    全てのユーザレコードを表示します。
OPER_HELP_CMD_LOGONNEWS
	    LOGONNEWS   ログオン時に表示するメッセージを編集します。
OPER_HELP_CMD_RANDOMNEWS
	    RANDOMNEWS  ログオン時にランダムに表示するメッセージを編集します。
OPER_HELP_CMD_OPERNEWS
	    OPERNEWS    オペレータに表示するメッセージを編集します。
OPER_HELP_CMD_SESSION
	    SESSION     指定されたホストからのセッションを表示します。
OPER_HELP_CMD_EXCEPTION
	    EXCEPTION   セッション制限からの除外リストを編集します。
OPER_HELP_CMD_NOOP
	    NOOP        サーバの O:lines を一時的に削除します。

OPER_HELP_CMD_JUPE
	    JUPE        サーバを "Jupiter" にします。
OPER_HELP_CMD_IGNORE
	    IGNORE      サービスの無視リストを編集します。
OPER_HELP_CMD_SET
	    SET         各種グローバルサービスオプションを設定します。
OPER_HELP_CMD_RELOAD
	    RELOAD      サービスの設定ファイルをリロードします。
OPER_HELP_CMD_UPDATE
	    UPDATE      今すぐサービスデータベースをディスクから読み出します。
OPER_HELP_CMD_RESTART
	    RESTART     データベースを保存して、サービスを再起動します。
OPER_HELP_CMD_QUIT
	    QUIT        保存せずにサービスを終了します。
OPER_HELP_CMD_SHUTDOWN
	    SHUTDOWN    保存してサービスを終了します。
OPER_HELP_CMD_DEFCON
	    DEFCON      DefCon システムを設定します。
OPER_HELP_CMD_CHANKILL
	    CHANKILL    チャンネルに参加中のユーザ全員を AKILL します。
OPER_HELP_CMD_OLINE
	    OLINE       オペレータフラグをユーザに与えます。
OPER_HELP_CMD_UMODE
	    UMODE       ユーザモードを変更します。
OPER_HELP_CMD_SVSNICK
	    SVSNICK     強制的にユーザのニックネームを変更します。
OPER_HELP_CMD_MODLOAD
	    MODLOAD     モジュールをロードします。
OPER_HELP_CMD_MODUNLOAD
	    MODUNLOAD   モジュールをアンロードします。
OPER_HELP_CMD_MODINFO
	    MODINFO     ロード済みモジュールに関する情報を表示します。
OPER_HELP_CMD_MODLIST
	    MODLIST     ロード済みモジュールを表示します。

OPER_HELP
	%S に関連するコマンド:

OPER_HELP_LOGGED
	注意: %S 宛てのコマンドは全て監視されています!

OPER_HELP_GLOBAL
	構文: GLOBAL message
	
	管理者によるネットワークにいるユーザ全員宛てのメッセージ送信
	を許可します。このメッセージの送信者は %s になります。

OPER_HELP_STATS
	構文: STATS [AKILL | ALL | RESET | MEMORY | UPLINK]
	
	オプションを指定しない場合、現在のユーザ数と IRC オペレータ
	(サービスオペレータを除く) の数、サービスが開始されてからの
	最大ユーザ数、サービス開始からの経過時間を表示します。
	
	AKILL オプションが指定された場合は、現在の AKILL リスト
	のサイズと AKILL 失効時間の初期値を表示します。
	
	RESET オプションが指定された場合は、現在ネットワーク上に
	いるユーザ数の最大値をリセットします。
	
	MEMORY オプションが指定された場合、サービスが使用している
	メモリーの情報を表示します。このオプションを使用すると、
	大きなネットワークで稼働中のサービスが一時的に停止します。
	
	UPLINK オプションが指定された場合は、現在のサーバで
	稼働中の Anope が使用するアップリンクサーバを表示します。
	
	ALL オプションは ユーザと稼働時間の統計に加えて、
	MEMORY と UPLINK オプションで表示される情報を表示します。
	
	UPTIME オプションは STATS オプションのエイリアスです。

OPER_HELP_OPER
	構文: OPER ADD nick
	      OPER DEL {nick | entry-num | list}
	      OPER LIST [mask | list]
	      OPER CLEAR
	
	サービスのルート管理者がサービスオペレータリストへのニックネームを
	追加したりや削除したりすることを許可します。サービスオペレータリストに
	載っているニックネームのユーザか %s で特定できるユーザは
	サービスオペレータコマンドを使用することができます。
	
	OPER ADD コマンドは指定されたニックネームを
	サービスオペレータリストに追加します。
	
	OPER DEL コマンドは指定されたニックネームをサービスオペレータリスト
	から削除します。エントリ番号が指定された場合は、番号に対応したエントリが
	削除されます (LIST コマンドの例を参照してください)。
	
	OPER LIST コマンドはサービスオペレータリストを表示します。
	ワイルドカードマスクが指定された場合は、このマスクに該当するエントリのみ
	が表示されます。エントリ番号が指定された場合は、それらに対応した
	エントリのみが表示されます。例えば以下のような感じです。

	   OPER LIST 2-5,7-9
	      2 から 5 番目と 7 から 9 番目のサービスオペレータの
	      エントリを表示します。
	      
	OPER CLEAR コマンドはサービスオペレータリストの
	全てのエントリを削除します。
	
	IRC オペレータであれば誰でも OPER LIST 型のコマンドを使用できます。

OPER_HELP_ADMIN
	構文: ADMIN ADD nick
	      ADMIN DEL {nick | entry-num | list}
	      ADMIN LIST [mask | list]
	      ADMIN CLEAR
	
	サービスルートがサービス管理者リストにニックネームを追加したり
	削除したりすることを許可します。サービス管理者リストに載っている
	ニックネームのユーザか %s で特定できるユーザは
	サービス管理者コマンドを使用することができます。
	
	ADMIN ADD コマンドは指定されたニックネームを
	サービス管理者リストに追加します。
	
	ADMIN DEL コマンドは指定されたニックネームをサービス管理者リストから
	削除します。エントリ番号が指定された場合は、番号に対応したエントリが
	削除されます (LIST コマンドの例を参照してください)。
	
	ADMIN LIST コマンドはサービス管理者リストを表示します。
	ワイルドカードマスクが指定された場合は、このマスクに該当する
	エントリのみが表示されます。エントリ番号が指定された場合は、
	それらに対応したエントリのみが表示されます。例えば以下のような感じです。

	   ADMIN LIST 2-5,7-9
	      2 から 5 番目と 7 から 9 番目のサービス管理者の
	      エントリを表示します。
	      
	ADMIN CLEAR コマンドはサービス管理者リストの
	全てのエントリを削除します。
	
	IRC オペレータであれば誰でも ADMIN LIST 型のコマンドを使用できます。
	それ以外のコマンドはは サービスルート でないと使えません。

OPER_HELP_IGNORE
	構文: IGNORE {ADD|DEL|LIST|CLEAR} [time] [nick | mask]
	
	サービス管理者はサービスが指定されたニックネームかマスク
	をある期間か次に再起動するまで無視するようにします。
	時間フォーマットに単位を指定しない場合の初期値は秒です。
	以下のようにして、単位を指定できます。使用可能な単位は、
	s で秒、 m で分、h で時間、d で日と解釈
	されます。これらを組み合わせて使用することはできません。
	サービスが、あるユーザを恒久的に無視するためには、0 を指定
	してください。 mask を指定する場合は、この形式は
	user@host の形か、nick!user@host の形で指定できます。
	どちらでもない形式の場合はニックネームとみなされます。
	また、ワイルドカードを使うこともできます。
	
	IRC オペレータを無視することはできません。

OPER_HELP_MODE
	構文: MODE channel modes
	
	サービスオペレータがチャンネルモードを変更できるように
	します。引数は /MODE コマンドと同じです。

OPER_HELP_UMODE
	構文: UMODE user modes
	
	スーパー管理者がユーザモードを変更できるようにします。
	引数は /MODE コマンドと同じです。

OPER_HELP_OLINE
	構文: OLINE user flags
	
	スーパー管理者がユーザにオペレータフラグを
	与えることができるようにします。フラグは "+" か "-" で
	はじめ、フラグを剥奪する場合は "-" を使用してください。

OPER_HELP_CLEARMODES
	構文: CLEARMODES channel [ALL]
	
	チャンネルに対して、全てのバイナリモード (i、k、l、m、n、p、s、t) を解除し、
	バンリストを空にします。ALL が指定された場合は、さらにチャンネル内の
	ユーザからチャンネルオペレータと voices ユーザの権利を剥奪します。

OPER_HELP_KICK
	構文: KICK channel user reason
	
	スタッフがチャンネルからユーザをキックします。引数は普通の /KICK
	コマンドと同じ形式です。キック理由の先頭に KICK コマンドを使った IRC
	オペレータのニックネームを追記した内容でキックメッセージが送信されます。
	例えば、下のような感じです:
	
	*** %S が SpamMan をチャンネル #my_channel からキックしました。(Alcan (Flood 攻撃))

OPER_HELP_SVSNICK
	構文: SVSNICK nick newnick
	
	ユーザのニックネームを nick から newnick に変更します。
	この機能はスーパー管理者以外は使えません。

OPER_HELP_AKILL
	構文: AKILL ADD [+expiry] mask reason
	      AKILL DEL {mask | entry-num | list}
	      AKILL LIST [mask | list]
	      AKILL VIEW [mask | list]
	      AKILL CLEAR
	
	サービスオペレータが AKILL リストを編集することを許可します。
	AKILL マスクに該当するユーザが接続してきた場合、サービスは
	このユーザを KILL します。加えて、サーポート済みのサーバに
	おいては、全てのサーバに対してこのユーザにマッチする K-line
	を追加するように働きかけます。
	
	AKILL ADD コマンドは user@host か ip で指定されたマスク
	とその理由 (必須) を AKILL リストに追加します。
	失効時間 expiry は整数の後に単位 d (日)、h (時間)、
	m (分) の内 1 つを続けて指定します。ただし、1h30m のように
	組み合わせて指定することはできません。単位を指定しなかった場合、
	日として解釈されます (+30 という指定は 30 日間と解釈されます)。
	永久に失効されない AKILL を追加するには +0 と指定してください。
	ユーザマスクを + から始めた場合、失効時間をを必ず指定して
	ください。指定しない場合は初期値が指定されたものと解釈されます。
	AKILL 失効時間の初期値は STATS AKILL コマンドで指定できます。
	
	AKILL DEL コマンドは指定されたマスクを AKILL リストから
	削除します。エントリ番号が指定された場合は、番号に該当する
	エントリを削除します (LIST コマンドの例を参照してください)。
	
	AKILL LIST コマンドは AKILL リストを表示します。
	ワイルドカードを指定した場合、このワイルドカードに該当
	するエントリのみが表示されます。エントリ番号を指定した場合、
	番号に該当するエントリのみが表示されます。例えば、以下の様な感じです:

	   AKILL LIST 2-5,7-9
	      2 から 5 番と 7 から 9 番の AKILL
	      エントリを表示します。
	      
	AKILL VIEW は AKILL LIST を詳細にしたもので、
	AKILL の user@host か ip マスクとその理由に加えて、
	このエントリを追加したメンバー、追加日、失効日を表示します。
	
	AKILL CLEAR コマンドは AKILL リストを削除します。
	
OPER_HELP_SGLINE
	構文: SGLINE ADD [+expiry] mask:reason
	      SGLINE DEL {mask | entry-num | list}
	      SGLINE LIST [mask | list]
	      SGLINE VIEW [mask | list]
	      SGLINE CLEAR
	
	サービスオペレータが SGLINE リストを編集することを許可します。
	SGLINE マスクに該当する実ユーザ名を持つユーザが接続してきた
	場合、サービスはこのユーザの IRC セッションを破棄します。
	
	SGLINE ADD コマンドは実ユーザ名マスク
	とその理由 (必須) を SGLINE リストに追加します。
	失効時間 expiry は整数の後に単位 d (日)、h (時間)、
	m (分) の内 1 つを続けて指定します。ただし、1h30m のように
	組み合わせて指定することはできません。単位を指定しなかった場合、
	日として解釈されます (+30 という指定は 30 日間と解釈されます)。
	永久に失効されない SGLINE を追加するには +0 と指定してください。
	実ユーザ名マスクを + から始めた場合、失効時間をを必ず指定して
	ください。指定しない場合は初期値が指定されたものと解釈されます。
	SGLINE 失効時間の初期値は STATS AKILL コマンドで指定できます。
	注意: 実ユーザ名はスペースを含むことがあるので、マスクと
	理由の区切りはスペースの変わりにコロンを使ってください。
	
	SGLINE DEL コマンドは指定されたマスクを SGLINE リストから
	削除します。エントリ番号が指定された場合は、番号に該当する
	エントリを削除します (LIST コマンドの例を参照してください)。
	
	SGLINE LIST コマンドは SGLINE リストを表示します。
	ワイルドカードを指定した場合、このワイルドカードに該当
	するエントリのみが表示されます。エントリ番号を
	指定した場合、番号に該当するエントリのみが表示されます。
	例えば、以下の様です。

	   SGLINE LIST 2-5,7-9
	      2 から 5 番と 7 から 9 番の SGLINE エントリを表示します。
	      
	SGLINE VIEW は SGLINE LIST を詳細にしたもので、
	SGLINE の実ユーザ名マスクとその理由に加えて、
	このエントリを追加したメンバー、追加日、失効日を表示します。
	
	SGLINE CLEAR コマンドは SGLINE リストを削除します。
	

OPER_HELP_SQLINE
	構文: SQLINE ADD [+expiry] mask reason
	      SQLINE DEL {mask | entry-num | list}
	      SQLINE LIST [mask | list]
	      SQLINE VIEW [mask | list]
	      SQLINE CLEAR
	
	サービスオペレータが SQLINE リストを編集することを許可します。
	SQLINE マスクに該当するニックネームを持つユーザが接続してきた
	場合、サービスはこのユーザの IRC セッションを破棄します。

	指定されたマスクが # から始まる場合、サービスは
	該当するチャンネルを作れないようにします
	(ただし、IRCd がこれをサポートしていなければなりません)。
	
	SQLINE ADD コマンドはマスク
	とその理由 (必須) を SQLINE リストに追加します。
	失効時間 expiry は整数の後に単位 d (日)、h (時間)、
	m (分) の内 1 つを続けて指定します。ただし、1h30m のように
	組み合わせて指定することはできません。単位を指定しなかった場合、
	日として解釈されます (+30 という指定は 30 日間と解釈されます)。
	永久に失効されない SQLINE を追加するには +0 と指定してください。
	マスクを + から始めた場合、失効時間をを必ず指定して
	ください。指定しない場合は初期値が指定されたものと解釈されます。
	SQLINE 失効時間の初期値は STATS AKILL コマンドで指定できます。
	
	SQLINE DEL コマンドは指定されたマスクを SQLINE リストから
	削除します。エントリ番号が指定された場合は、番号に該当する
	エントリを削除します (LIST コマンドの例を参照してください)。
	
	SQLINE LIST コマンドは SQLINE リストを表示します。
	ワイルドカードを指定した場合、このワイルドカードに該当
	するエントリのみが表示されます。エントリ番号を指定した場合、
	番号に該当するエントリのみが表示されます。例えば、以下の様な感じです。

	   SQLINE LIST 2-5,7-9
	      2 から 5 番と 7 から 9 番の SQLINE エントリを表示します。
	      
	SQLINE VIEW は SQLINE LIST を詳細にしたもので、
	SQLINE のマスクとその理由に加えて、
	このエントリを追加したメンバー、追加日、失効日を表示します。
	
	SQLINE CLEAR コマンドは SQLINE リストを削除します。

OPER_HELP_SZLINE
	構文: SZLINE ADD [+expiry] mask reason
	      SZLINE DEL {mask | entry-num | list}
	      SZLINE LIST [mask | list]
	      SZLINE VIEW [mask | list]
	      SZLINE CLEAR
	
	サービスオペレータが SZLINE リストを編集することを許可します。
	SZLINE マスクに該当する IP アドレスを持つユーザが接続してきた
	場合、サービスはこのユーザの IRC セッションを破棄します。
	(破棄は IP アドレスがドメインネームポインタのリソースレコード
	を持っているか否かにかかわらず行われます。)
	
	SZLINE ADD コマンドは IP アドレスマスク
	とその理由 (必須) を SZLINE リストに追加します。
	失効時間 expiry は整数の後に単位 d (日)、h (時間)、
	m (分) の内 1 つを続けて指定します。ただし、1h30m のように
	組み合わせて指定することはできません。単位を指定しなかった場合、
	日として解釈されます (+30 という指定は 30 日間と解釈されます)。
	永久に失効されない SZLINE を追加するには +0 と指定してください。
	マスクを + から始めた場合、失効時間をを必ず指定して
	ください。指定しない場合は初期値が指定されたものと解釈されます。
	SZLINE 失効時間の初期値は STATS AKILL コマンドで指定できます。
	
	SZLINE DEL コマンドは指定されたマスクを SZLINE リストから
	削除します。エントリ番号が指定された場合は、番号に該当する
	エントリを削除します。(LIST コマンドの例を参照してください)
	
	SZLINE LIST コマンドは SZLINE リストを表示します。
	ワイルドカードを指定した場合、このワイルドカードに該当
	するエントリのみが表示されます。エントリ番号を
	指定した場合、番号に該当するエントリのみが表示されます。
	例えば、以下の様です。

	   SZLINE LIST 2-5,7-9
	      2 から 5 番と 7 から 9 番の SZLINE エントリを表示します。
	      
	SZLINE VIEW は SZLINE LIST を詳細にしたもので、
	SZLINE の IP アドレスマスクとその理由に加えて、
	このエントリを追加したメンバー、追加日、失効日を表示します。
	
	SZLINE CLEAR コマンドは SZLINE リストを削除します。

OPER_HELP_SET
	構文: SET option setting

	サービスのグローバルオプションを設定します。現在
	定義されているオプションは:
	    READONLY   読み込み専用か読み書きモードにします。
	    LOGCHAN    チャンネルにログメッセージを流します。
	    DEBUG      デバッグモードの有効無効を切り替えます。
	    NOEXPIRE   非失効モードの有効無効を切り替えます。
	    SUPERADMIN スーパー管理者モードの有効無効を切り替えます。
	    SQL        sql モードの有効無効を切り替えます。
	    IGNORE     無視モードの有効無効を切り替えます。
	    LIST       オプションを表示します。

OPER_HELP_SET_READONLY
	構文: SET READONLY {ON | OFF}
	
	読み込み専用モードの有効無効を切り替えます。読み込み専用モード
	では、通常のユーザはチャンネルとニックネームのリスト等、全ての
	サービスデータを変更できません。適当な権利を持つ IRC オペレータ
	だけがサービスの AKILL リストを変更して、ニックネームや
	チャンネルを削除追加できます。ただし、この時行った変更は
	サービスを終了するか再起動する前に読み取り専用モードを
	無効にしない限り保存されません。
	
	このオプションはコマンドラインオプション -readonly を
	指定することと等価です。

OPER_HELP_SET_LOGCHAN
	構文: SET LOGCHAN {ON | OFF}

	この設定を有効にすると、サービスはサービスログをログファイルに
	保存するのと同時に、これを特定のチャンネルに出力します。
	その場合は、変数 LogChannel をサービス設定ファイル内で定義
	しなければいけません。
	
	このオプションはコマンドラインオプション -logchan を
	指定することと等価です。
	
	注意: このオプションは強固なセキュリティの観点からすれば
	好ましくありません。有効にする場合に、ログ出力チャンネルを
	公開チャンネルにすることはお勧めできません。

OPER_HELP_SET_DEBUG
	構文: SET DEBUG {ON | OFF | num}
	
	デバッグモードの有効無効を切り替えます。デバッグモードでは
	サービスが送受信する全てのデータに加えて多くのデバッグメッセージ
	がログファイルに記録されます。num を指定した場合は、
	デバッグモードが有効になり、デバッグレベルが num に
	設定されます。

	このオプションはコマンドラインオプション -debug を
	指定することと等価です。

OPER_HELP_SET_NOEXPIRE
	構文: SET NOEXPIRE {ON | OFF}

	非失効モードの有効無効を切り替えます。非失効モードでは、
	ニックネーム、チャンネル、akill、例外が失効を迎えることは
	ありません。これは非失効モードが無効になるまで続きます。

	このオプションはコマンドラインオプション -noexpire を
	指定することと等価です。

OPER_HELP_SET_SUPERADMIN
	構文: SET SUPERADMIN {ON | OFF}

	スーパー管理者モードの有効無効を切り替えます。スーパー管理者
	モードでは、全てのチャンネルの "founder" になったりできます。

	このオプションは通常は無効化すべきもので、必要に迫られた
	場合にのみ有効化されるべきです。また、不要になったら無効にする
	べきです。

OPER_HELP_SET_SQL
	構文: SET SQL {ON | OFF}

	サービスの運用中に SQL サーバを終了させる場合に使用します。

OPER_HELP_SET_IGNORE
	構文: SET IGNORE {ON | OFF}

	無視モードのの有効無効を切り替えます。

OPER_HELP_SET_LIST
	構文: SET LIST

	%S の設定を表示します。

OPER_HELP_NOOP
	構文: NOOP SET server
	      NOOP REVOKE server

	NOOP SET は指定された server の O:lines を
	全て削除し、サーバ設定が書き換えられるのを防ぐために、
	現在このサーバにいる IRC オペレータ全員を kill します。

	NOOP REVOKE は指定された server の
	削除された O:line を復活させます。

	注意: サービスは server が正しいかどうか
	チェックしません。

OPER_HELP_JUPE
	構文: JUPE server [reason]
	
	サービスがサーバを守るようにします。これはつまり、
	サービスに接続する身代わりサーバを作り、同じ名前を持つ
	サーバがサービスに接続できないようにします。この身代わり
	サーバを削除するには通常の SQUIT コマンドが使えます。
	理由を指定した場合は、サーバ情報フィールドにこれを
	書き込みます。理由を指定しなかった場合、サーバ情報フィールド
	には "Juped by <nick>" のようにこの身代わりサーバを
	つくったユーザのニックネームを含む内容を書き込みます。

OPER_HELP_RAW
	構文: RAW text
	
	サービスが接続しているサーバにテキストを直接送信します。
	このコマンドは使用に制約があり、使い方を間違えると
	ネットワークに大きな障害を引き起こす可能性があります。
	あなたがしようとしている事を十分に理解していない状態では
	このコマンドを絶対に使用しないでください!

OPER_HELP_UPDATE
	構文: UPDATE
	
	コマンドを送信後できるだけ早く、サービスに全ての
	データベースファイルをアップデートさせます。
	
OPER_HELP_RELOAD
	構文: RELOAD

	サービスに設定ファイルを再読み込みさせます。いくつかの命令
	を有効化させるには、サービスの再起動が必要な場合があります。
	例えば、サービスのニックネーム、セッション制限の有効化等です。

OPER_HELP_QUIT
	構文: QUIT
	
	サービスを今すぐ終了させます。データベースのセーブは
	行われません。このコマンドはメモリ上に展開したデータベースの
	コピーに障害が発生したおそれがあり、これを保存しないほうが良い
	場合を除いて使用するべきではありません。通常の終了の際には
	SHUTDOWN コマンドを使用してください。


OPER_HELP_SHUTDOWN
	構文: SHUTDOWN
	
	サービスに全てのデータベースをセーブさせ、サービスを終了します。

OPER_HELP_RESTART
	構文: RESTART
	
	サービスに全てのデータベースを保存させ、サービスを再起動します
	(つまり、終了後直ぐに起動処理を繰り返します)。

OPER_HELP_CHANLIST
	構文: CHANLIST [{pattern | nick} [SECRET]]
	
	IRC ネットワーク上に現在あるチャンネルを、
	登録済みか否かにかかわらず全て表示します。

	pattern が指定された場合は、該当するチャンネルのみを
	表示します。ニックネームが指定された場合は、このユーザが
	参加しているチャンネルのみを表示します。SECRET が指定
	された場合は、pattern に該当するチャンネルのうち
	+s か +p モードのチャンネルのみを表示します。

OPER_HELP_USERLIST
	構文: USERLIST [{pattern | channel} [INVISIBLE]]
	
	IRC ネットワーク上に現在いるユーザを、
	登録済みニックネームが否かにかかわらず全員表示します。
	
	pattern (フォーマットは nick!user@host) が指定された場合、
	これに該当するユーザのみを表示します。channel が指定された場合、
	このチャンネルに参加しているユーザのみを表示します。INVISIBLE
	が指定された場合は、+i フラグが立っているユーザのみを表示します。

OPER_HELP_MODLOAD
	構文: MODLOAD FileName
	
	モジュールディレクトリ内から FileName で指定した
	モジュールをロードします。

OPER_HELP_MODUNLOAD
	構文: MODUNLOAD FileName
	
	モジュールディレクトリ内から FileName で指定した
	モジュールをアンロードします。

OPER_HELP_MODINFO
	構文: MODINFO FileName
	
	指定されたロード済みモジュールの情報を表示します。

OPER_HELP_MODLIST
	構文: MODLIST [Core|3rd|protocol|encryption|supported|qatested]
	
	ロード済みモジュールを全て表示します。


###########################################################################
#
# BotServ help messages
#
###########################################################################
BOT_HELP_CMD_BOTLIST
	    BOTLIST        ボットのリストを表示します。
BOT_HELP_CMD_ASSIGN
	    ASSIGN         ボットをチャンネルに召喚します。
BOT_HELP_CMD_SET
	    SET            ボットのオプションを設定します。
BOT_HELP_CMD_KICK
	    KICK           キッカーを設定します。
BOT_HELP_CMD_BADWORDS
	    BADWORDS       禁止ワードリストを設定します。
BOT_HELP_CMD_ACT
	    ACT            ボットを "/me" コマンドと同様に発言させます。
BOT_HELP_CMD_INFO
	    INFO           チャンネルかボットに関する BotServ の情報を表示します。
BOT_HELP_CMD_SAY
	    SAY            指定されたテキストを指定されたチャンネルで発言させます。
BOT_HELP_CMD_UNASSIGN
	    UNASSIGN       ボットをチャンネルから削除します。
BOT_HELP_CMD_BOT
	    BOT            ボットリストを設定しま。

BOT_HELP
	%S 機能を使うと、あなたのチャンネルにボットを
	召喚できるようになります。これはボットを動かしたり、
	設定できないユーザ、またはボットを動かすのを禁止
	しているネットワークにいるユーザのための機能です。
	使用可能なコマンドは以下に示してあります。これらを
	使用するには、 %R%S command と入力してください。
	また、それぞれのコマンドに関するより詳しい情報が、
	%R %S HELP command と入力すれは見られます。
	

BOT_HELP_FOOTER
	チャンネルに参加中のユーザ数が %d 人を超えると、
	ボットがチャンネルに参加できるようになります。

	
BOT_HELP_BOTLIST
	構文: BOTLIST
	
	このネットワークで使えるボットのリストを表示する。
	
BOT_HELP_ASSIGN
	構文: ASSIGN chan nick
	
	ボットを chan で指定したチャンネルに召喚します。
	召喚の後には、チャンネルのポリシーに合うように
	ボットを設定してください。
	
BOT_HELP_UNASSIGN
	構文: UNASSIGN chan
	
	ボットをチャンネルから削除します。このコマンドを使うと、
	ボットはチャンネルから削除れます。ただし、このチャンネル
	に関連したボットの設定は保存されますので、再度召喚した時に
	改めて設定をやり直す必要はありません。
	
BOT_HELP_INFO
	構文: INFO {chan | nick}
	
	チャンネルかボットに関する %S の情報を表示します。
	チャンネルを指定した場合には、有効になっているキッカー
	等の情報を表示します。ニックネームを指定した場合には、
	召喚された時間や召喚されているチャンネルの数等の、
	ボットに関する情報を表示します。
	
BOT_HELP_SET
	構文: SET (channel | bot) option parameters
	
	ボットのオプションを設定します。option に指定できるのは以下です。
	
	    DONTKICKOPS      オペレータのキックを禁止します。
	    DONTKICKVOICES   voices ユーザのキックを禁止します。
	    GREET            歓迎メッセージを有効にします。
	    FANTASY          fantaisist モードを有効にします。
	    SYMBIOSIS        ボットが実ユーザの如く振る舞うよう設定します。
	
	より詳しい情報を見るには、%R%S HELP SET option と
	入力してください。

	注意: このコマンドを使うには SET コマンドで設定した
	レベル以上の権限が必要です。
	
BOT_HELP_SET_DONTKICKOPS
	構文: SET channel DONTKICKOPS {ON|OFF}
	
	あるチャンネルでのオペレータ保護 モード
	の有効無効を切り替えます。これが有効で、
	オペレータが NOKICK レベルにマッチしない場合、
	ボットはこのオペレータをキックしません。
	
	
BOT_HELP_SET_DONTKICKVOICES
	構文: SET channel DONTKICKVOICES {ON|OFF}
	
	あるチャンネルでのvoices ユーザ保護 モードの有効無効を
	切り替えます。これが有効で、voices ユーザが NOKICK レベルに
	マッチしない場合、ボットはこの voices ユーザをキックしません。
	
BOT_HELP_SET_FANTASY
	構文: SET channel FANTASY {ON|OFF}

	あるチャンネルでのファンタジーモードの有効無効を
	切り替えます。これが有効な場合ユーザはチャンネル内で
	以下のコマンドが使えます。!op、!deop、!voice、!devoice、
	!kick、!kb、!unban、!seen。それぞれの使い方は、
	引数にニックネームをつけたり外したり、
	理由をつけたりして色々試してみてください。
	
	注意することは、ユーザがファンタジーモード
	コマンドを使用するためには、そのユーザが
	FANTASIA 以上の権限レベルとそれぞれのコマンドに
	対応した権限レベルを持っていなければいけません。
	例えば、!op を使うためには、ユーザは OPDEOP
	レベル以上の権限を持っていなければいけません。

BOT_HELP_SET_GREET
	構文: SET channel GREET {ON|OFF}
	
	あるチャンネルでの歓迎モードの有効無効を
	切り替えます。これが有効の場合、ボットはチャンネル
	に参加したユーザに歓迎メッセージを表示します。

BOT_HELP_SET_SYMBIOSIS
	構文: SET channel SYMBIOSIS {ON|OFF}
	
	あるチャンネルでの symbiosis モードの有効無効を
	切り替えます。これが有効の場合、ボットはチャンネルで
	通常 %s ができることがすべてできるようになります。
	例えば、MODE コマンド、KICK コマンド、メッセージの送信等です。

BOT_HELP_KICK
	構文: KICK channel option parameters
	
	キッカーを設定します。option に指定できるのは以下です。
	
	    BOLDS         太字キッカーの有効無効を切り替えます。
	    BADWORDS      禁止ワードキッカーの有効無効を切り替えます。
	    CAPS          大文字キッカーの有効無効を切り替えます。
	    COLORS        色文字キッカーの有効無効を切り替えます。
	    FLOOD         Flood 攻撃キッカーの有効無効を切り替えます。
	    REPEAT        繰り返しキッカーの有効無効を切り替えます。
	    REVERSES      反転文字キッカーの有効無効を切り替えます。
	    UNDERLINES    下線キッカーの有効無効を切り替えます。
	
	より詳しい情報を見るには、%R%S HELP KICK option と
	入力してください。
	
	注意: このコマンドを使うには SET コマンドで設定した
	レベル以上の権限が必要です。

BOT_HELP_KICK_BOLDS
	構文: KICK channel BOLDS {ON|OFF} [ttb]

	太字キッカーの有効無効を切り替えます。
	有効の場合、ボットは太字を発言したユーザをキックします。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_COLORS
	構文: KICK channel COLORS {ON|OFF} [ttb]

	色文字キッカーの有効無効を切り替えます。
	有効の場合、ボットは色文字を発言したユーザをキックします。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_REVERSES
	構文: KICK channel REVERSES {ON|OFF} [ttb]

	反転文字キッカーの有効無効を切り替えます。
	有効の場合、ボットは色文字を発言したユーザをキックします。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_UNDERLINES
	構文: KICK channel UNDERLINES {ON|OFF} [ttb]

	下線キッカーの有効無効を切り替えます。
	有効の場合、ボットは下線を発言したユーザをキックします。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_CAPS
	構文: KICK channel CAPS {ON|OFF} [ttb [min [percent]]]

	大文字キッカーの有効無効を切り替えます。
	有効の場合、ボットは大文字を発言したユーザをキックします。

	少なくとも min 文字以上の大文字があり、
	1 行に含まれる大文字が percent%% 以上の場合に、
	キックします (これらが指定されなかった場合の
	初期値は 10 文字と 25 %% です)。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_FLOOD
	構文: KICK channel FLOOD {ON|OFF} [ttb [ln [secs]]]

	flood 攻撃キッカーの有効無効を切り替えます。
	有効の場合、ボットは flood 攻撃をしたユーザをキックします。
	secs 秒間にln 回の発言した場合を flood 攻撃
	とみなします (これらが指定されなかった場合の
	初期値は 10 秒間に 6 発言です)。
	
	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_REPEAT
	構文: KICK #channel REPEAT {ON|OFF} [ttb [num]]

	繰り返しキッカーの有効無効を切り替えます。
	有効の場合、ボットは num 回同じ内容を
	繰り返し発言をしたユーザをキックします
	(この値が指定されなかった場合の初期値は 3 回です)。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。

BOT_HELP_KICK_BADWORDS
	構文: KICK #channel BADWORDS {ON|OFF} [ttb]

	禁止ワードキッカーの有効無効を切り替えます。
	有効の場合、ボットは 禁止ワードを発言したユーザをキックします。

	BADWORDS コマンドで禁止ワードを登録できます。
	より詳しい情報を見るには %R%S HELP BADWORDS と
	入力してください。

	あるユーザが ttb 回キックされるとこのユーザをバンします。
	バン機能を使わない場合は ttb を指定しないでください。
	
BOT_HELP_BADWORDS
	構文: BADWORDS channel ADD word [SINGLE | START | END]
	      BADWORDS channel DEL {word | entry-num | list}
	      BADWORDS channel LIST [mask | list]
	      BADWORDS channel CLEAR
	
	指定されたチャンネルの禁止ワードリストを編集します。
	禁止ワードとは、禁止ワードキッカーが有効な場合にキック対象
	となるワードのことです。より詳しい情報を見るには、
	%R%S HELP KICK BADWORDS と入力してください。
	
	BADWORDS ADD コマンドは、指定されたワードを禁止ワード
	リストに追加します。SINGLE が指定された場合、単独で
	禁止ワードが使用された発言についてキックします。START が
	指定された場合、word から始まる発言についてキック
	します。END が指定された場合、word で終わる発言に
	ついてキックします。これらの指定が無い場合、発言の
	どの位置で word を使用しても、キックします。

	BADWORDS DEL コマンドは、指定されたワードを禁止ワード
	リストから削除します。エントリ番号が指定された場合は、
	この番号に対応した禁止ワードを削除します (LIST の例も
	参照してください)。
	
	BADWORDS LIST コマンドは、禁止ワードリストを表示します。
	ワイルドカードマスクが指定された場合は、このマスクに該当する
	エントリのみを表示します。エントリ番号が指定された場合は、
	指定されたものだけを表示します。例えば、以下のように指定できます。

	   BADWORDS #channel LIST 2-5,7-9
	      2 から 5 番目と 7 から 9 番目の禁止ワードエントリを表示します。
	      
	BADWORDS CLEAR コマンドは、禁止ワードリストに登録されている
	全てのエントリを削除します。

BOT_HELP_SAY
	構文: SAY channel text
	
	ボットに指定したチャンネル宛に指定した内容で発言させます。

BOT_HELP_ACT
	構文: ACT channel text
	
	ボットに "/me" コマンドと同じように、指定したチャンネル
	宛に指定した内容で発言させます。


BOT_SERVADMIN_HELP_BOT
	構文: BOT ADD nick user host real
	      BOT CHANGE oldnick newnick [user [host [real]]]
	      BOT DEL nick
	
	サービス管理者にボットを作成、変更、削除する許可を与えます。
	ユーザは自分のチャンネルでこのボットを使うことができます。
	
	BOT ADD は指定されたニックネーム、ユーザ名、ホスト名、
	実名でボットを追加します。設定項目に関して整合性のチェック
	は行われないので、十分気をつけてください。
	BOT CHANGE は指定されたニックネーム、ユーザ名、ホスト名、
	実名でボットの設定内容を変更します。ボットの削除を行わないので、
	全ての設定内容は引き継がれます。
	BOT DEL は指定されたボットをボットリストから削除します。
	
	注意: 登録済みのニックネームで新規にボットを作成することは
	できません。引数で指定されたニックネームを使用中の未登録
	ユーザがいる場合は、このユーザを kill します。

BOT_SERVADMIN_HELP_SET
	
	これらのオプションはサービス管理者のみが使えます。
	
	    NOBOT            チャンネルでのボット召喚を禁止します。
	    PRIVATE          IRC オペレータ以外のユーザによる
	                        ボット召喚を禁止します。

BOT_SERVADMIN_HELP_SET_NOBOT
	構文: SET channel NOBOT {ON|OFF}
	
	指定したチャンネルへのボット召喚を許可するか否かを設定します。
	このオプションを有効にした時点で、チャンネルにボットが
	召喚されている場合、自動的にボットを削除します。

BOT_SERVADMIN_HELP_SET_PRIVATE
	構文: SET bot-nick PRIVATE {ON|OFF}

	IRC オペレータでないユーザによるボット召喚を禁止
	するか否かを設定します。

###########################################################################
#
# HostServ messages
#
###########################################################################
HOST_EMPTY
	バーチャルホストのリストに登録レコードがありません。
HOST_ENTRY
	#%d ニックネーム:%s、バーチャルホスト:%s (%s - %s)
HOST_IDENT_ENTRY
	#%d ニックネーム:%s、バーチャルホスト:%s@%s (%s - %s)
HOST_SET
	%s のバーチャルホストを %s に設定しました。
HOST_IDENT_SET
	%s のバーチャルホストを %s@%s に設定しました。
HOST_SETALL
	グループ %s のバーチャルホストを %s に設定しました。
HOST_DELALL
	グループ %s のバーチャルホストを削除しました。
HOST_DELALL_SYNTAX
	DELALL <nick>.
HOST_IDENT_SETALL
	グループ %s のバーチャルホストを %s@%s に設定しました。
HOST_SET_ERROR
	バーチャルホストを有効なホストマスクの形式にしてください。
HOST_SET_IDENT_ERROR
	バーチャルホスト ident を有効な ident の形式にしてください。
HOST_SET_TOOLONG
	エラー! バーチャルホストが長すぎます。%d 文字以下にしてください。
HOST_SET_IDENTTOOLONG
	エラー! ident が長すぎます。%d 文字以下にしてください。
HOST_NOREG
	ユーザ %s が nickserv のデータベースに見つかりません。
HOST_SET_SYNTAX
	SET <nick> <hostmask>.
HOST_SETALL_SYNTAX
	SETALL <nick> <hostmask>.
HOST_DENIED
	アクセスが拒否されました。
HOST_NOT_ASSIGNED
	このニックネームにバーチャルホストを割り当てるには、サーバオペレータに連絡を取ってください。
HOST_ACTIVATED
	あなたのバーチャルホスト %s を有効化しました。
HOST_IDENT_ACTIVATED
	あなたのバーチャルホスト %s@%s を有効化しました。
HOST_ID
	サービスが起動中かどうか確認してください。
HOST_NOT_REGED
	バーチャルホストを登録してください。
HOST_DEL
	バーチャルホスト %s を削除しました。
HOST_DEL_SYNTAX
	DEL <nick>.
HOST_OFF
	バーチャルホストを削除し、通常の隠匿ホストに変更しました。
HOST_OFF_UNREAL
	バーチャルホストを削除しました。/mode %s +%s と入力すれば、通常の隠匿ホストに変更できます
HOST_NO_VIDENT
	あなたの IRC デーモンは vIdent をサポートしていません。サポートしている場合はバグレポートを送ってください
HOST_GROUP
	グループ %s に所属しているすべてのバーチャルホストを %s に変更しました
HOST_IDENT_GROUP
	グループ %s に所属しているすべてのバーチャルホストを %s@%s に変更しました
HOST_LIST_FOOTER
	レコードを全て表示しました (レコード数: %d)
HOST_LIST_RANGE_FOOTER
	%d 番から %d 番のレコードを表示しました
HOST_LIST_KEY_FOOTER
	キー %s に該当するレコードを表示しました (レコード数: %d)

###########################################################################
#
# HostServ Help messages
#
###########################################################################
HOST_HELP_CMD_ON
	    ON          登録済みのバーチャルホストを使います。

HOST_HELP_CMD_OFF
	    OFF         登録済みのバーチャルホストを使いません。

HOST_HELP_CMD_GROUP
	    GROUP       グループ内のユーザ全員のバーチャルホストを同じものに変更します。

HOST_HELP_CMD_SET
	    SET         別のユーザのバーチャルホストを設定します。

HOST_HELP_CMD_SETALL
	    SETALL      グループ内のユーザ全員のバーチャルホストを設定します

HOST_HELP_CMD_DEL
	    DEL         別のユーザのバーチャルホストを削除します。

HOST_HELP_CMD_DELALL
	    DELALL      グループ内のユーザ全員のバーチャルホストを削除します。

HOST_HELP_CMD_LIST
	    LIST        バーチャルホストエントリを表示します。

HOST_OPER_HELP
	使われていません。

HOST_ADMIN_HELP
	使われていません。

HOST_HELP
	%S 機能に関するコマンド:

HOST_HELP_ON
	構文: ON

	使用中のニックネームに対応したバーチャルホスト設定を有効化します。
	このコマンドを使うと、/whois で表示される本当の IP アドレスの
	代わりに設定されたバーチャルホストが表示されます。

HOST_HELP_SET
	構文: SET <nick> <hostmask>.

	指定されたニックネームに対応したバーチャルホストを設定します。
	IRC デーモンが vIdents をサポートしていない場合は
	SET <nick> <ident>@<hostmask> のように入力すると、
	バーチャルホストに加えて idents も設定できます。

HOST_HELP_DELALL
	構文: DELALL <nick>.

	指定されたニックネームと同じグループに所属する
	ユーザ全員のバーチャルホスト設定を削除します。


HOST_HELP_SETALL
	構文: SETALL <nick> <hostmask>.

	指定されたニックネームと同じグループに所属する
	ユーザ全員のバーチャルホストを設定します。
	IRC デーモンが vIdents をサポートしていない場合は
	SETALL <nick> <ident>@<hostmask> のように入力すると、
	バーチャルホストに加えて idents も設定できます。

	* 注意、このコマンドはあるグループに所属するユーザの
	バーチャルホストの初期値を設定するものではありません。
	そのため、このコマンドを実行した後にグループに追加
	されたユーザのバーチャルホストを変更するには、
	再度同じコマンドを入力してください。

HOST_HELP_OFF
	構文: OFF

	使用中のニックネームに対応したバーチャルホスト設定を無効化します。
	このコマンドを使うと、/whois で本当の IP アドレスが表示されます。


HOST_HELP_DEL
	構文: DEL <nick>

	指定したニックネームに対応したバーチャルホストの設定
	をデータベースから削除します。

HOST_HELP_LIST
	構文: LIST [<key>|<#X-Y>]

	登録済みバーチャルホストを表示します。
	Key が指定された場合は、ニックネームかバーチャルホスト
	が <key> にマッチするエントリのみを表示します。
	例えば、Rob* と指定された場合は、"Rob" が含まれる
	すべてのエントリを表示します。

	#X-Y 形式が指定された場合は、X から Y 番目のエントリを表示
	します。例えば、#1-3 と指定された場合は、ニックネーム/
	バーチャルホストエントリの最初から 3 つを表示します。

	1 回に表示するエントリ数の上限値は NSListMax で指定します。


HOST_HELP_GROUP
	構文: GROUP
	
	現在のニックネームと同じグループに所属するユーザ全員
	のバーチャルホストを現在のニックネームに対応した
	バーチャルホストに設定します。

OPER_SVSNICK_UNSUPPORTED
	ご迷惑を掛けて申し訳ありません、このネットワークで SVSNICK は使えません。

OPER_SQLINE_UNSUPPORTED
	ご迷惑を掛けて申し訳ありません、このネットワークで SQLINE は使えません。

OPER_SVSO_UNSUPPORTED
	ご迷惑を掛けて申し訳ありません、このネットワークで OLINE は使えません。

OPER_UMODE_UNSUPPORTED
	ご迷惑を掛けて申し訳ありません、このネットワークで UMODE は使えません。

OPER_SUPER_ADMIN_NOT_ENABLED
	SuperAdmin の設定が services.conf の中で無効になっています。

