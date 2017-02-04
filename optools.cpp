//OpTools module by QueenElsa
//DESCRIPTION:
//This module will log all op activities that occur in channels you
//are currently joined to. Based on code from sample module. 
//USAGE:
// -- Compile module.
// -- Load into ZNC (/znc loadmod X-autojoin) 
//Enjoy! 
#include <znc/Chan.h>
#include <znc/IRCNetwork.h>
#include <znc/Modules.h>
#include <znc/Client.h>
using std::vector;
class Coptools : public CModule {
public:
		MODCONSTRUCTOR(Coptools) { PutModule ("[SYSTEM] Module loaded. Monitoring activities in all joined channels."); }
		virtual ~Coptools() { PutModule ("[SYSTEM] Module unloaded. No longer monitoring activities in all joined channels."); }
	    void OnIRCConnected() override { PutModule("[SYSTEM/CONNECTION] Connected to network."); }
		void OnIRCDisconnected() override { PutModule("[SYSTEM/CONNECTION] Disconnected from network."); }
		/*void OnChanPermission(const CNick& OpNick, const CNick& Nick,
							  CChan& Channel, unsigned char uMode, bool bAdded,
							  bool bNoChange) override {
			PutModule(((bNoChange) ? "[0] [" : "[1] [") + OpNick.GetNick() +
					  "] set mode [" + Channel.GetName() +
					  ((bAdded) ? "] +" : "] -") + CString(uMode) + " " +
					  Nick.GetNick());
		}*/

		void OnOp(const CNick& OpNick, const CNick& Nick, CChan& Channel,
				  bool bNoChange) override {
			PutModule("[CHANNEL/OP] " + OpNick.GetNick() +
					  " opped " + Nick.GetNick() + " in " + Channel.GetName());
		}

		void OnDeop(const CNick& OpNick, const CNick& Nick, CChan& Channel,
					bool bNoChange) override {
			PutModule("[CHANNEL/DEOP] " + OpNick.GetNick() +
					  " deopped " + Nick.GetNick() + " in " + Channel.GetName());
		}

		void OnVoice(const CNick& OpNick, const CNick& Nick, CChan& Channel,
					 bool bNoChange) override {
						PutModule("[CHANNEL/VOICE] " + OpNick.GetNick() +
					  " voiced " + Nick.GetNick() + " in " + Channel.GetName());
		}

		void OnDevoice(const CNick& OpNick, const CNick& Nick, CChan& Channel,
					   bool bNoChange) override {
			PutModule("[CHANNEL/OP] " + OpNick.GetNick() +
					  " devoiced " + Nick.GetNick() + " in " + Channel.GetName());
		}

		/* void OnRawMode(const CNick& OpNick, CChan& Channel, const CString& sModes,
					   const CString& sArgs) override {
			PutModule("* " + OpNick.GetNick() + " sets mode: " + sModes + " " +
					  sArgs + " (" + Channel.GetName() + ")");
		} */
		void OnKick(const CNick& OpNick, const CString& sKickedNick, CChan& Channel,
                const CString& sMessage) override {
        PutModule("[CHANNEL/KICK] " + OpNick.GetNick() + " kicked " + sKickedNick +
                  " from " + Channel.GetName() + " for: " +
                  sMessage);
		}

		void OnQuit(const CNick& Nick, const CString& sMessage,
					const vector<CChan*>& vChans) override {
			PutModule("[USER/QUIT] " + Nick.GetNick() + " (" + Nick.GetIdent() + "!" +
					  Nick.GetHost() + ") disconnected from the network. Quit Message: " + sMessage);
		}
		
		void OnJoin(const CNick& Nick, CChan& Channel) override {
        PutModule("[USER/JOIN] " + Nick.GetNick() + " (" + Nick.GetIdent() + "!" +
                  Nick.GetHost() + ") has joined: " + Channel.GetName());
		}

		void OnPart(const CNick& Nick, CChan& Channel,
					const CString& sMessage) override {
			PutModule("[USER/PART] " + Nick.GetNick() + " (" + Nick.GetIdent() + "!" +
					  Nick.GetHost() + ") has left: " + Channel.GetName());
		}
		
		EModRet OnTopic(CNick& Nick, CChan& Channel, CString& sTopic) override {
        PutModule("[CHANNEL/TOPIC] " + Nick.GetNick() + " has changed the topic in " +
                  Channel.GetName() + " to: " + sTopic);

			return CONTINUE;
		}

		EModRet OnUserTopic(CString& sTarget, CString& sTopic) override {
			PutModule("[CHANNEL/TOPIC (YOU] " + GetClient()->GetNick() + " has change the topic in " +
					  sTarget + " to: " + sTopic);

			return CONTINUE;
		}
		
		EModRet OnStatusCommand(CString& sCommand) override {
        if (sCommand.Equals("TEST")) {
            PutModule("This was successful.");
            return HALT;
        }

        return CONTINUE;
    }
};
NETWORKMODULEDEFS(Coptools, "Log op activities in all joined channels.")
