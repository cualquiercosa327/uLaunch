#include <ipc/ipc_IPrivateService.hpp>
#include <dmi/dmi_DaemonMenuInteraction.hpp>
#include <am/am_LibraryApplet.hpp>

extern ams::os::Mutex g_LastMenuMessageLock;
extern dmi::MenuMessage g_LastMenuMessage;

namespace ipc {

    ams::Result PrivateService::Initialize(const ams::sf::ClientProcessId &client_pid) {
        if(!this->initialized) {
            u64 program_id = 0;
            UL_RC_TRY(pminfoGetProgramId(&program_id, client_pid.process_id.value));

            const auto last_menu_program_id = am::LibraryAppletGetMenuProgramId();
            // If Menu hasn't been launched it's program ID will be 0 (invalid), thus a single (program_id != last_menu_program_id) check isn't enough
            // If any of the IDs is invalid, something unexpected is happening...
            if((last_menu_program_id == 0) || (program_id == 0) || (program_id != last_menu_program_id)) {
                return ipc::ResultInvalidProcess;
            }

            this->initialized = true;
        }
        
        return ResultSuccess;
    }

    ams::Result PrivateService::GetMessage(ams::sf::Out<dmi::MenuMessage> out_msg) {
        if(!this->initialized) {
            return ipc::ResultInvalidProcess;
        }

        std::scoped_lock lk(g_LastMenuMessageLock);
        out_msg.SetValue(g_LastMenuMessage);
        g_LastMenuMessage = dmi::MenuMessage::Invalid;
        return ResultSuccess;
    }

}