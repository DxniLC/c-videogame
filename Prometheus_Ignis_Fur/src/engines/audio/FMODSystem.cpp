#include <engines/audio/FMODSystem.hpp>

#include <unistd.h>

void FMODSystem::initialize()
{
    void *extraDriverData = NULL;
    system = systemManaged(nullptr, releaseSystem);
    FMOD_STUDIO_SYSTEM *rawPtr = system.get();
    result = FMOD_Studio_System_Create(&rawPtr,FMOD_VERSION); 

    system.reset(rawPtr);
    

    FMOD_Studio_System_GetCoreSystem(system.get(),&coreSystem);

    
    FMOD_System_SetSoftwareFormat(coreSystem, 0 , FMOD_SPEAKERMODE_5POINT1, 0);


    FMOD_System_Set3DSettings(coreSystem,0,0,0);

    result = FMOD_Studio_System_Initialize(system.get(), 1024, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL | FMOD_INIT_PROFILE_ENABLE , extraDriverData);
    std::cout << "1" << result;
    const char *error = FMOD_ErrorString(result);
    std::cout << "3" << error;
    masterBank = NULL;
    result = FMOD_Studio_System_LoadBankFile(system.get(),"media/audio/banks/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);
    std::cout << "2:" << result << "2:";
    error = FMOD_ErrorString(result);
    std::cout << "3" << error << "fin";


    FMOD_STUDIO_BANK *stringsBank = NULL;
    FMOD_Studio_System_LoadBankFile(system.get(),"media/audio/banks/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

    FMOD_STUDIO_BANK *sfxBank = NULL;
    FMOD_Studio_System_LoadBankFile(system.get(),"media/audio/banks/SFX.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);
}

AudioInstance *FMODSystem::loadSoundEvent(const char *eventSound, float volumePlus)
{

    AudioInstance *audioResult{nullptr};

    eventDescription = nullptr;
    eventInstance = nullptr;
    
    std::string path{eventSound};

    std::string eventPath{"event:/" + path};

    result = FMOD_Studio_System_GetEvent(system.get(),eventPath.c_str(), &eventDescription);

    result = FMOD_Studio_EventDescription_CreateInstance(eventDescription,&eventInstance);

    FMOD_Studio_EventInstance_SetVolume(eventInstance , 1 + volumePlus);

    auto itr = audioMaps.find(path);
    if (itr != audioMaps.end())
    {
        audioResult = itr->second.emplace_back(std::make_unique<FMODInstance>(eventInstance)).get();
    }
    else
    {
        
        VectorInstances eventVector;

        eventVector.emplace_back(std::make_unique<FMODInstance>(eventInstance));

        audioResult = audioMaps.emplace(path, std::move(eventVector)).first->second.back().get();

    }

   

    return audioResult;
}

void FMODSystem::clearSoundEvent(const char *pathSound)
{
    auto itr = audioMaps.find(pathSound);
    if (itr != audioMaps.end())
    {
        audioMaps.erase(itr);
    }
}

AudioInstance * FMODSystem::getSoundEvent(const char *pathSound)
{
    std::string path{pathSound};
    auto itr = audioMaps.find(path);
    if (itr != audioMaps.end() && !itr->second.empty())
    {
        return itr->second[0].get();
    }

    return nullptr;

}

void FMODSystem::setListener(glm::vec3 pos, glm::vec3 direction,glm::vec3 up)
{

    

    FMOD_3D_ATTRIBUTES l_listenerAttributes;

    l_listenerAttributes.position.x = -pos.x;
    l_listenerAttributes.position.y = pos.y;
    l_listenerAttributes.position.z = pos.z;

    l_listenerAttributes.velocity.x = 0.0f;
    l_listenerAttributes.velocity.y = 0.0f;
    l_listenerAttributes.velocity.z = 0.0f;

    l_listenerAttributes.forward.x = -direction.x;
    l_listenerAttributes.forward.y = direction.y;
    l_listenerAttributes.forward.z = direction.z;

    l_listenerAttributes.up.x = -up.x;
    l_listenerAttributes.up.y = up.y;
    l_listenerAttributes.up.z = up.z;


    FMOD_Studio_System_SetListenerAttributes(system.get(),0,&l_listenerAttributes,nullptr);

    

    
    
}

void FMODSystem::setVCAvolume(float vol, const char *pathVCA)
{
    FMOD_STUDIO_VCA *vca{nullptr};

    std::string path{pathVCA};

    std::string VCAPath{"vca:/" + path};


    FMOD_Studio_System_GetVCA(system.get(),VCAPath.c_str(),&vca);


    FMOD_Studio_VCA_SetVolume(vca,vol);


}



void FMODSystem::update()
{

    FMOD_Studio_System_Update(system.get());
}