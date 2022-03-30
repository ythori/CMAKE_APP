

#include "_createTemplateTest.h"
#include "CreateTemplate.h"
#include "../Shared/Constants.h"
#include "../Shared/DataProc.h"

void createTemplateTest() {
    echo("Create template");

    std::string folder_name = "MusicHSMM";
    std::string class_name = "MusicHSMM";
    std::shared_ptr<CreateTemplate> CT = std::make_shared<CreateTemplate>(folder_name, class_name);

    CT->execute();


}
