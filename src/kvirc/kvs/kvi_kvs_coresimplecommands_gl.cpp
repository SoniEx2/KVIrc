//=============================================================================
//
//   File : kvi_kvs_coresimplecommands_gl.cpp
//   Created on Fri 31 Oct 2003 00:04:25 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
//=============================================================================

#define __KVIRC__

#include "kvi_kvs_coresimplecommands.h"

#include "kvi_kvs_timermanager.h"
#include "kvi_kvs_asyncdnsoperation.h"
#include "kvi_kvs_script.h"

#include "kvi_fileutils.h"
#include "kvi_ircconnection.h"
#include "kvi_ircconnectionserverinfo.h"
#include "kvi_locale.h"

#include "kvi_out.h"

namespace KviKvsCoreSimpleCommands
{
	/*
		@doc: halt
		@type:
			command
		@title:
			halt
		@syntax:
			halt
		@short:
			Stops the execution of the current script
		@description:
			Stops the execution of the current script.[br]
			Note that this is different from [cmd]return[/cmd]: halt doesn't
			set the return value (use [cmd]setreturn[/cmd] for that) and
			in the event handlers this command may have additional semantics:
			refer to the specific event documentation for more info.[br]
			In fact , this command should be used [b]only[/b] in the event handlers,
			use [cmd]return[/cmd] in any other context.[br]
	*/

	KVSCSC(halt)
	{
		KVSCSC_pContext->setHaltCalled();
		return false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: help
		@type:
			command
		@title:
			help
		@syntax:
			help [-s] <topic>
		@short:
			Shows a help page
		@description:
			Shows a help page about <topic>.[br]
			<topic> can be a command name , or an identifier.[br]
			If <topic> starts with a slash , it is assumed to be a full path
			to a html file to be displayed in the help browser.[br]
			This command is a shortcut for the [cmd]help.open[/cmd] module command.[br]
		@examples:
			[example]
			help run
			help help
			help index
			help $mask
			help /home/pragma/myfile.html
			[/example]
	*/

	static bool file_in_path(QStringList &pathlist,QString &file)
	{
		for(QStringList::Iterator it = pathlist.begin();it != pathlist.end();++it)
		{
			QString tmp = *it;
			tmp.append('/');
			tmp.append(file);
			KviFileUtils::adjustFilePath(tmp);
			if(KviFileUtils::fileExists(tmp))return true;
		}
		return false;
	}

	KVSCSC(help)
	{
		QString szTopic;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("topic",KVS_PT_STRING,KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING,szTopic)
		KVSCSC_PARAMETERS_END

		szTopic.trimmed();
		if(szTopic.endsWith(";"))
		{
			szTopic.truncate(szTopic.length() - 1);
			szTopic.trimmed();
		}

		QString szCommand;
		if(szTopic.isEmpty())szCommand = "help.open index.html";
		else {
/* FIXME removed this usage if the evil goto. better if reimplemented directly in the help module
			
			bool bWasFunc = false;
			if(szTopic[0] == QChar('$'))
			{
				bWasFunc = true;
				szTopic.remove(0,1);
			}
	
			QString szOriginal = szTopic;
	
			QString topik = szTopic;
			QStringList pl = Q3MimeSourceFactory::defaultFactory()->filePath();

			bool bLowCaseTried = false;
			bool bFound = KviFileUtils::fileExists(szTopic);
	
try_again:
			if((!bFound) && bWasFunc)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("fnc_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("cmd_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("doc_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("event_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("module_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("widget_");
				bFound = file_in_path(pl,topik);
			}
	
			if(!bFound)
			{
				topik = szTopic;
				topik += ".html";
				topik.prepend("class_");
				bFound = file_in_path(pl,topik);
			}
			
			if(!bFound && !bLowCaseTried)
			{
				// try again with lowercase
				szTopic = szTopic.toLower();
				bLowCaseTried = true;
				goto try_again;
			}
			
			if(!bFound)
			{
				// let the help.open function decide...
				topik = szOriginal;
				topik += ".html";
			}
	
			szCommand = "help.open ";
			szCommand += topik;
*/
			szCommand = "help.open " + szTopic;
		}

		// We don't care about the return value...
		KviKvsScript s("help",szCommand);
		s.run(KVSCSC_pContext->window());

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: host
		@type:
			command
		@title:
			host
		@syntax:
			host [-i] [-a] <dnsquery:string>
		@short:
			DNS lookup
		@switches:
			!sw: --ipv6 | -i
			Causes the command to run in IPv6 mode
			!sw: --any | -a
			Causes the command to run in unspecified mode and lookup both IPv4 and IPv6 addresses
		@description:
			Starts a DNS lookup for the <dnsquery> (hostname) and prints
			the results in the current window.[br]
			The -i switch causes the command to execute
			in IpV6 mode (and lookup ONLY IpV6 hosts!).[br]
			The -a switch causes the command to run in "unspecified" mode
			and return any available address: IpV4 or Ipv6.[br]
			This command also performs reverse lookups (if you pass an IP address as <hostname>).[br]
		@examples:
			[example]
			host localhost
			host irc.flashnet.it
			host -i irc.flashnet.it
			host -i A0E0:FFFF:::1010:1111:E0E0:A0A0
			host -a 127.0.0.1
			host 255.41.31.23
			host -a irc.stealth.net
			[/example]
		@seealso:
			[cmd]ahost[/cmd]
	*/

	KVSCSC(host)
	{
		QString szQuery;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("dnsquery",KVS_PT_NONEMPTYSTRING,0,szQuery)
		KVSCSC_PARAMETERS_END

		KviDns::QueryType queryType = KviDns::IpV4;
		if(KVSCSC_pSwitches->find('i',"ipv6"))queryType = KviDns::IpV6;
		if(KVSCSC_pSwitches->find('a',"any"))queryType = KviDns::Any;

		KviKvsAsyncDnsOperation * op = new KviKvsAsyncDnsOperation(
						KVSCSC_pContext->window(),
						szQuery,
						queryType);

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: include
		@type:
			command
		@title:
			include
		@syntax:
			include [-q] [-e] [-f] [-r] <filename:string> [<parameter1:variant> [<parameter2:variant> [...]]]
		@short:
			A builtin alias for parse
		@description:
			This is a builtin alias for [cmd]parse[/cmd] that gives scripts a "more C-ish" look :)
		@seealso:
			[cmd]parse[/cmd]
	*/

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: info
		@type:
			command
		@title:
			info
		@syntax:
			info [target server]
		@short:
			Requests the server daemon info from a server
		@description:
			Requests server daemon information from the specified server or the current server if no [target server] is specified.[br]
			This command is a [doc:rfc2821wrappers]RFC2821 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2821 wrapper

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: invite
		@type:
			command
		@title:
			invite
		@syntax:
			invite <nickname> <channel>
		@short:
			Invites an user to a channel
		@description:
			Invites the specified user to the <channel>.[br]
			This command is a [doc:rfc2821wrappers]RFC2821 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RCF2821 wrapper

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: join
		@type:
			command
		@title:
			join
		@syntax:
			join <channel list> [key list]
		@short:
			Joins the specified channels
		@description:
			Joins the channels specified in <channel list> by using the
			optional keys in the [key list].[br]
			This command is "server based"; you will see its results only
			after the server acknowledges it.[br]
			This command is [doc:connection_dependant_commands]connection dependant[/doc].[br]
		@examples:
			[example]
			join #kvirc
			join #linux-it,#xmms,#kde linus
			[/example]
	*/

	KVSCSC(join)
	{
		QString szChans,szKeys;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("chans",KVS_PT_NONEMPTYSTRING,0,szChans)
			KVSCSC_PARAMETER("keys",KVS_PT_STRING,KVS_PF_OPTIONAL,szKeys)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		QStringList slChans = szChans.split(",",QString::SkipEmptyParts);

		QString szChanTypes = KVSCSC_pConnection->serverInfo()->supportedChannelTypes();
		for(QStringList::Iterator it=slChans.begin();it != slChans.end();++it)
		{
			if(!(*it).isEmpty())
			{
				bool bFound = false;
				int id = 0;
				QChar f = (*it)[0];
				while((id < szChanTypes.length()) && !bFound)
				{
					if(f == szChanTypes[id])bFound = true;
					id++;
				}
				if(!bFound)
					(*it).prepend("#");
			}
		}

		szChans = slChans.join(",");
		KviQCString szEncodedChans = KVSCSC_pConnection->encodeText(szChans);

		if(szKeys.isEmpty())
		{
			if(!(KVSCSC_pConnection->sendFmtData("JOIN %s",szEncodedChans.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		} else {
			KviQCString szEncodedKeys  = KVSCSC_pConnection->encodeText(szKeys);
			if(!(KVSCSC_pConnection->sendFmtData("JOIN %s %s",szEncodedChans.data(),szEncodedKeys.data())))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: kick
		@type:
			command
		@title:
			kick
		@syntax:
			kick <user:string> [reason:string]
		@short:
			Kicks an user from a channel
		@description:
			Kicks the specified <user> from the current channel.[br]
			This command is a simple wrapper over the IRC KICK command.
			This command is [doc:connection_dependant_commands]connection dependant[/doc].
		@examples:
			[example]
			kick Pragma Go coding in Pascal!
			[/example]
	*/

	KVSCSC(kick)
	{
		QString szUser;
		QString szReason;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("user",KVS_PT_NONEMPTYSTRING,0,szUser)
			KVSCSC_PARAMETER("reason",KVS_PT_STRING,KVS_PF_OPTIONAL | KVS_PF_APPENDREMAINING,szReason)
		KVSCSC_PARAMETERS_END

		KVSCSC_REQUIRE_CONNECTION

		if(KVSCSC_pWindow->type() != KVI_WINDOW_TYPE_CHANNEL)
		{
			KVSCSC_pContext->warning(__tr2qs("The current window is not a channel"));
			return false;
		}

		KviQCString szC = KVSCSC_pConnection->encodeText(KVSCSC_pWindow->target());
		KviQCString szU = KVSCSC_pConnection->encodeText(szUser);
		KviQCString szR = KVSCSC_pConnection->encodeText(szReason);

		if(szR.isEmpty())
		{
			if(!KVSCSC_pConnection->sendFmtData("KICK %s %s",szC.data(),szU.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		} else {
			if(!KVSCSC_pConnection->sendFmtData("KICK %s %s :%s",szC.data(),szU.data(),szR.data()))
				return KVSCSC_pContext->warningNoIrcConnection();
		}

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: killtimer
		@title:
			killtimer
		@type:
			command
		@short:
			Stops a timer
		@syntax:
			killtimer [-a]
			killtimer [-q] [name]
		@switches:
			!sw: -a | --active
			Requests to kill all the active timers
			!sw: -q | --quiet
			Does not print warnings if the specified timer is not running
		@description:
			The first version (with -a) kill all the running timers.[br]
			The second verdion kills a running timer named [name].[br]
			The timer has been started by the [cmd]timer[/cmd].[br]
			[name] can be omitted if this command is called from a [cmd]timer[/cmd] callback:
			in this case the current timer will be scheduled for killing immediately
			after it has returned control to KVIrc.
		@seealso:
			[cmd]timer[/cmd], [fnc]$isTimer[/fnc], [cmd]listtimers[/cmd]
	*/

	KVSCSC(killtimer)
	{
		QString szName;
		KVSCSC_PARAMETERS_BEGIN
			KVSCSC_PARAMETER("name",KVS_PT_STRING,KVS_PF_OPTIONAL,szName)
		KVSCSC_PARAMETERS_END

		if(KVSCSC_pSwitches->find('a',"active"))
		{
			KviKvsTimerManager::instance()->deleteAllTimers();
			return true;
		}

		if(szName.isEmpty())
		{
			if(!KviKvsTimerManager::instance()->deleteCurrentTimer())
			{
				if(!KVSCSC_pSwitches->find('q',"quiet"))
					KVSCSC_pContext->warning(__tr2qs("Timer name omitted but there is not current timer (this is not a timer callback)"));
			}
		} else {
			if(!KviKvsTimerManager::instance()->deleteTimer(szName))
			{
				if(!KVSCSC_pSwitches->find('q',"quiet"))
					KVSCSC_pContext->warning(__tr2qs("Can't kill the timer '%Q' since it is not running"),&szName);
			}
		}
		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: leave
		@type:
			command
		@title:
			leave
		@syntax:
			leave [-k] [-q] [-s] <channel list> [part message]
		@short:
			Alias for part
		@description:
			Builtin alias for the command [cmd]part[/cmd].
	*/
	// Internally aliased to leave

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: links
		@type:
			command
		@title:
			links
		@syntax:
			links
		@short:
			Lists server links
		@description:
			List server links.[br]
			The syntax of the parameters of this command is different on many servers.[br]
			This command is a [doc:rfc2821wrappers]RFC2821 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2821 wrapper

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: list
		@type:
			command
		@title:
			list
		@syntax:
			list [ <channel> *( "," <channel> ) [ <target> ] ]
		@short:
			Lists channels
		@description:
			List channels.[br]
			The syntax of the parameters of this command is different on many servers.[br]
			This command is a [doc:rfc2821wrappers]RFC2821 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2821 wrapper

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: listtimers
		@title:
			listtimers
		@type:
			command
		@short:
			Lists the active timers
		@syntax:
			listtimers
		@description:
			Lists the currently active timers
		@seealso:
			[cmd]timer[/cmd], [fnc]$isTimer[/fnc], [cmd]killtimer[/cmd]
	*/

	KVSCSC(listtimers)
	{
		KviPointerHashTable<QString,KviKvsTimer> * pTimerDict = KviKvsTimerManager::instance()->timerDict();

		if(!pTimerDict)
			return true;
		
		KviPointerHashTableIterator<QString,KviKvsTimer> it(*pTimerDict);
		
		KVSCSC_pContext->window()->outputNoFmt(KVI_OUT_VERBOSE,__tr2qs("List of active timers"));
		
		unsigned int uCnt = 0;
		
		while(KviKvsTimer * pTimer = it.current())
		{
			QString szName = pTimer->name();
			QString szLifetime;
			switch(pTimer->lifetime())
			{
				case KviKvsTimer::Persistent:
					szLifetime = __tr2qs("Persistent");
				break;
				case KviKvsTimer::WindowLifetime:
					szLifetime = __tr2qs("WindowLifetime");
				break;
				case KviKvsTimer::SingleShot:
					szLifetime = __tr2qs("SingleShot");
				break;
				default:
					szLifetime = __tr2qs("Unknown");
				break;
			}
			QString szDelay;
			szDelay.setNum(pTimer->delay());
			QString szWindow;
			szWindow = pTimer->window() ? pTimer->window()->id() : __tr2qs("None");

			KVSCSC_pContext->window()->output(KVI_OUT_VERBOSE,
					"Timer \"%Q\": Lifetime: %Q, Delay: %Q, Window: %Q",
					&szName,&szLifetime,&szDelay,&szWindow
				);

			uCnt++;
			++it;
		}

		KVSCSC_pContext->window()->output(KVI_OUT_VERBOSE,__tr2qs("Total: %u timers running"),uCnt);

		return true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
		@doc: lusers
		@type:
			command
		@title:
			lusers
		@syntax:
			lusers [ <mask> [ <target> ] ]
		@short:
			Requests statistics about users on IRC
		@description:
			The LUSERS command is used to get statistics about the size of the
			IRC network.[br]If no parameter is given, the reply will be about the
			whole net.[br]If a <mask> is specified, then the reply will only
			concern the part of the network formed by the servers matching the
			mask.[br]Finally, if the <target> parameter is specified, the request
			is forwarded to that server which will generate the reply.[br]
			This command is a [doc:rfc2821wrappers]RFC2821 command wrapper[/doc]; see that document for more information.[br]
	*/
	// RFC2821 wrapper
};
