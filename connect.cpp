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
	*  OnConnect module by QueenElsa • Version 1.0
	* --------------------------------------------------
	*  Channel: #development @ irc.undernet.org
	*  Contact: QueenElsa@undernet.org
	* --------------------------------------------------
	*  Performs commands automatically when a client is
	*  reattached to ZNC. Based off of ClientNotify and
	*  Perform modules.
	* ==================================================
	*/

	#include <znc/IRCNetwork.h>
	#include <znc/znc.h>
	#include <znc/User.h>

	using std::set;

	class COnConnect : public CModule {
	protected:
		CString m_sMethod;
		bool m_bNewOnly{};
		bool m_bOnDisconnect{};

		set<CString> m_sClientsSeen;
		void Add(const CString& sCommand) {
			CString sOnConn = sCommand.Token(1, true);

			if (sOnConn.empty()) {
				PutModule("Usage: add <command>");
				return;
			}

			m_vOnConnect.push_back(ParseOnConnect(sOnConn));
			PutModule("Added!");
			Save();
		}

		void Del(const CString& sCommand) {
			u_int iNum = sCommand.Token(1, true).ToUInt();

			if (iNum > m_vOnConnect.size() || iNum <= 0) {
				PutModule("Illegal # Requested");
				return;
			} else {
				m_vOnConnect.erase(m_vOnConnect.begin() + iNum - 1);
				PutModule("Command Erased.");
			}
			Save();
		}

		void List(const CString& sCommand) {
			CTable Table;
			unsigned int index = 1;

			Table.AddColumn("Id");
			Table.AddColumn("OnConnect");
			Table.AddColumn("Expanded");

			for (VCString::const_iterator it = m_vOnConnect.begin(); it != m_vOnConnect.end(); ++it, index++) {
				Table.AddRow();
				Table.SetCell("Id", CString(index));
				Table.SetCell("OnConnect", *it);

				CString sExpanded = ExpandString(*it);

				if (sExpanded != *it) {
					Table.SetCell("Expanded", sExpanded);
				}
			}

			if (PutModule(Table) == 0) {
				PutModule("No commands in your OnConnect list.");
			}
		}

		void Execute(const CString& sCommand) {
			OnClientLogin();
			PutModule("OnConnect commands sent");
		}

		void Swap(const CString& sCommand) {
			u_int iNumA = sCommand.Token(1).ToUInt();
			u_int iNumB = sCommand.Token(2).ToUInt();

			if (iNumA > m_vOnConnect.size() || iNumA <= 0 || iNumB > m_vOnConnect.size() || iNumB <= 0) {
				PutModule("Illegal # Requested");
			} else {
				std::iter_swap(m_vOnConnect.begin() + (iNumA - 1), m_vOnConnect.begin() + (iNumB - 1));
				PutModule("Commands Swapped.");
				Save();
			}
		}

	public:
		MODCONSTRUCTOR(COnConnect) {
			AddHelpCommand();
			AddCommand("Add",     static_cast<CModCommand::ModCmdFunc>(&COnConnect::Add),
				"<command>", "Adds an OnConnect command to be sent to the server on client connect");
			AddCommand("Del",     static_cast<CModCommand::ModCmdFunc>(&COnConnect::Del),
				"<number>", "Delete an OnConnect command");
			AddCommand("List",    static_cast<CModCommand::ModCmdFunc>(&COnConnect::List),"", "List the OnConnect commands");
			AddCommand("Execute", static_cast<CModCommand::ModCmdFunc>(&COnConnect::Execute),"", "Send the OnConnect commands to the server now");
			AddCommand("Swap",    static_cast<CModCommand::ModCmdFunc>(&COnConnect::Swap),
				"<number> <number>", "Swap two OnConnect commands");
		}

		virtual ~COnConnect() {}

		CString ParseOnConnect(const CString& sArg) const {
			CString sOnConn = sArg;

			if (sOnConn.Left(1) == "/")
				sOnConn.LeftChomp();

			if (sOnConn.Token(0).Equals("MSG")) {
				sOnConn = "PRIVMSG " + sOnConn.Token(1, true);
			}

			if ((sOnConn.Token(0).Equals("PRIVMSG") ||
					sOnConn.Token(0).Equals("NOTICE")) &&
					sOnConn.Token(2).Left(1) != ":") {
				sOnConn = sOnConn.Token(0) + " " + sOnConn.Token(1)
					+ " :" + sOnConn.Token(2, true);
			}

			return sOnConn;
		}

		virtual bool OnLoad(const CString& sArgs, CString& sMessage) override {
			GetNV("OnConnect").Split("\n", m_vOnConnect, false);

			return true;
		}

		virtual void OnClientLogin() override {
			for (VCString::const_iterator it = m_vOnConnect.begin(); it != m_vOnConnect.end(); ++it) {
				PutIRC(ExpandString(*it));
			}
		}

		virtual CString GetWebMenuTitle() override { return "OnConnect"; }

		virtual bool OnWebRequest(CWebSock& WebSock, const CString& sPageName, CTemplate& Tmpl) override {
			if (sPageName != "index") {
				// only accept requests to index
				return false;
			}

			if (WebSock.IsPost()) {
				VCString vsOnConn;
				WebSock.GetRawParam("OnConnect", true).Split("\n", vsOnConn, false);
				m_vOnConnect.clear();

				for (VCString::const_iterator it = vsOnConn.begin(); it != vsOnConn.end(); ++it)
					m_vOnConnect.push_back(ParseOnConnect(*it));

				Save();
			}

			for (VCString::const_iterator it = m_vOnConnect.begin(); it != m_vOnConnect.end(); ++it) {
				CTemplate& Row = Tmpl.AddRow("OnConnectLoop");
				Row["OnConnect"] = *it;
			}

			return true;
		}

	private:
		void Save() {
			CString sBuffer = "";

			for (VCString::const_iterator it = m_vOnConnect.begin(); it != m_vOnConnect.end(); ++it) {
				sBuffer += *it + "\n";
			}
			SetNV("OnConnect", sBuffer);
		}

		VCString m_vOnConnect;
	};

	template<> void TModInfo<COnConnect>(CModInfo& Info) {
		Info.AddType(CModInfo::UserModule);
		Info.SetWikiPage("OnConnect");
	}

	NETWORKMODULEDEFS(COnConnect, "Keeps a list of commands to be executed when a client connects to ZNC.")