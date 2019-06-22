/*
* Copyright (C) 2004-2019 ZNC, see the NOTICE file for details.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
* 
* 
* ==================================================
*  BotStatus module by QueenElsa • Version 1.0
* --------------------------------------------------
*  Designed for use by the Undernet User-Committee
* --------------------------------------------------
*  Channel: #development @ irc.undernet.org
*  Contact: QueenElsa@undernet.org
* --------------------------------------------------
*  Announces the status of an attached bot. Based 
*  of ClientNotify module.
* ==================================================
*/


#include <znc/IRCNetwork.h>
#include <znc/znc.h>
#include <znc/User.h>

using std::set;

class CBotStatus : public CModule {
protected:
	CString m_sMethod;
	bool m_bOnDisconnect{};
	bool m_bOnConnect{};

	set<CString> m_sBotsSeen;

	void SaveSettings() {
		SetNV("method", m_sMethod);
		SetNV("ondisconnect", m_bOnDisconnect ? "1" : "0");
		SetNV("onconnect", m_bOnConnect ? "1" : "0");
	}

	void SendNotification(const CString& sMessage) {
		if(m_sMethod == "message") {
			PutIRC("PRIVMSG #Arendelle :" +sMessage);
			PutIRC("PRIVMSG #Uback :" +sMessage);
		}
		else if(m_sMethod == "notice") {
			PutIRC("NOTICE #Arendelle :" +sMessage);
			PutIRC("NOTICE #Uback :" +sMessage);
		}
	}

public:
	MODCONSTRUCTOR(CBotStatus) {
		AddHelpCommand();
		AddCommand("Method", static_cast<CModCommand::ModCmdFunc>(&CBotStatus::OnMethodCommand), "<message|notice|off>", "Sets the notify method");
		AddCommand("OnDisconnect", static_cast<CModCommand::ModCmdFunc>(&CBotStatus::OnDisconnectCommand), "<on|off>", "Turns notifications on disconnect only.");
		AddCommand("OnConnect", static_cast<CModCommand::ModCmdFunc>(&CBotStatus::OnConnectCommand), "<on|off>", "Turns notifications on connect only.");
		AddCommand("Show", static_cast<CModCommand::ModCmdFunc>(&CBotStatus::OnShowCommand), "", "Show the current settings");
	}

	bool OnLoad(const CString& sArgs, CString& sMessage) override {
		m_sMethod = GetNV("method");

		if(m_sMethod != "notice" && m_sMethod != "message" && m_sMethod != "off") {
			m_sMethod = "message";
		}

		// default = off for these:

		m_bOnDisconnect = (GetNV("ondisconnect") == "0");
		m_bOnConnect = (GetNV("onconnect") == "0");

		return true;
	}

	void OnClientLogin() override {
		if(m_bOnConnect) {
			SendNotification("[ONLINE] Bot services have been restored. I will respond to commands I receive from ops.");
		}
	}

	void OnClientDisconnect() override {
		if(m_bOnDisconnect) {
			SendNotification("[OFFLINE] Bot services are unavailable. I will not respond to any commands received. Please contact a User-Com Administrator.");
		}
	}

	void OnMethodCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true).AsLower();

		if (sArg != "notice" && sArg != "message" && sArg != "off") {
			PutModule("Usage: Method <message|notice|off>");
			return;
		}

		if (sArg == "off") {
			PutModule("MESSAGE METHOD CHANGED: Off - You will not be informed if your bot goes offline.");
			PutModule("WARNING: Status updates will remain turned off until you change the method to NOTICE or MESSAGE.");
		}

		if (sArg == "notice") {
			PutModule("MESSAGE METHOD CHANGED: Notice - You will be informed if your bot has gone offline via a channel notice.");
		}

		if (sArg == "message") {
			PutModule("MESSAGE METHOD CHANGED: Message - You will be informed if your bot has gone offline via a channel message.");
		}

		m_sMethod = sArg;
		SaveSettings();
	}

	void OnDisconnectCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true).AsLower();

		if (sArg.empty()) {
			PutModule("Usage: OnDisconnect <on|off>");
			return;
		}

		if (sArg == "off") {
			PutModule("DISCONNECT NOTIFICATION CHANGED: Off - You will not be informed if your bot goes offline.");
			PutModule("WARNING: This setting will remain turned off until you manually turn it back on.");
		}

		if (sArg == "on") {
			PutModule("DISCONNECT NOTIFICATION CHANGED: On - You will be informed if your bot goes offline.");
		}

		m_bOnDisconnect = sArg.ToBool();
		SaveSettings();
		PutModule("Saved.");
	}

		void OnConnectCommand(const CString& sCommand) {
		const CString& sArg = sCommand.Token(1, true).AsLower();

		if (sArg.empty()) {
			PutModule("Usage: OnConnect <on|off>");
			return;
		}

		if (sArg == "off") {
			PutModule("CONNECT NOTIFICATION CHANGED: Off - You will not be informed if your bot comes online.");
			PutModule("WARNING: This setting will remain turned off until you manually turn it back on.");
		}

		if (sArg == "on") {
			PutModule("CONNECT NOTIFICATION CHANGED: On - You will be informed if your bot comes online.");
		}

		m_bOnConnect = sArg.ToBool();
		SaveSettings();
		PutModule("Saved.");
	}

	void OnShowCommand(const CString& sLine) {
		PutModule("Current settings: Message Method: " + m_sMethod + ", Notify if bot disconnected: " + CString(m_bOnDisconnect) + ", Notify if bot reconnects: " + CString(m_bOnConnect));
	}
};

template<> void TModInfo<CBotStatus>(CModInfo& Info) {
	Info.SetWikiPage("BotStatus");
}

USERMODULEDEFS(CBotStatus, "Notifies you when your bot has come online or gone offline. Configurable.")

