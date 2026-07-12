#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/StartPosObject.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>
#include <vector>
#include <algorithm>

using namespace geode::prelude;

class $modify(SmartPosEditorUI, EditorUI) {

    struct StartState {
        int mode = 0;
        bool flip = false;
        bool mini = false;
        int speed = 1;
    };

    struct Fields {
        StartPosObject* pendingStartPos = nullptr;

        int lastMode = 0;
        bool lastFlip = false;
        bool lastMini = false;
        int lastSpeed = 1;

        bool hasPreviousState = false;
    };


    bool isPortal(int id) {
        return
            id == 12 ||
            id == 13 ||
            id == 47 ||
            id == 111 ||
            id == 660 ||
            id == 745 ||
            id == 1331 ||
            id == 1933 ||

            id == 10 ||
            id == 11 ||

            id == 99 ||
            id == 101 ||

            id == 200 ||
            id == 201 ||
            id == 202 ||
            id == 203 ||
            id == 1334;
    }


    void scheduleScan(StartPosObject* start, float delay) {

        if (!start)
            return;

        m_fields->pendingStartPos = start;

        this->unschedule(
            schedule_selector(
                SmartPosEditorUI::scanStartPos
            )
        );

        this->scheduleOnce(
            schedule_selector(
                SmartPosEditorUI::scanStartPos
            ),
            delay
        );
    }


    void moveObject(GameObject* object, CCPoint offset) {

        EditorUI::moveObject(object, offset);

        auto start = typeinfo_cast<StartPosObject*>(object);

        if (!start)
            return;

        scheduleScan(start, 1.5f);
    }


    void onCreateObject(int id) {

        EditorUI::onCreateObject(id);

        if (id != 31)
            return;


        auto objects = m_editorLayer->getAllObjects();

        if (!objects)
            return;


        StartPosObject* newest = nullptr;

        float greatestX = -FLT_MAX;


        for (auto obj : CCArrayExt<GameObject*>(objects)) {

            auto start = typeinfo_cast<StartPosObject*>(obj);

            if (!start)
                continue;


            if (start->getPositionX() > greatestX) {
                greatestX = start->getPositionX();
                newest = start;
            }
        }


        if (newest)
            scheduleScan(newest, 0.f);
    }


    void scanStartPos(float) {
        
        auto start = m_fields->pendingStartPos;

        if (!start)
            return;


        auto settings = start->m_startSettings;

        if (!settings)
            return;


        StartState state;


        float startX = start->getPositionX();


        auto objects = m_editorLayer->getAllObjects();

        if (!objects)
            return;


        std::vector<GameObject*> portals;


        for (auto obj : CCArrayExt<GameObject*>(objects)) {

            if (!obj)
                continue;


            if (!isPortal(obj->m_objectID))
                continue;


            if (obj->getPositionX() <= startX + 10.f)
                portals.push_back(obj);
        }


        std::sort(
            portals.begin(),
            portals.end(),
            [](GameObject* a, GameObject* b) {
                return a->getPositionX() < b->getPositionX();
            }
        );


        for (auto portal : portals) {

            switch (portal->m_objectID) {

                case 12:
                    state.mode = 0;
                    break;

                case 13:
                    state.mode = 1;
                    break;

                case 47:
                    state.mode = 2;
                    break;

                case 111:
                    state.mode = 3;
                    break;

                case 660:
                    state.mode = 4;
                    break;

                case 745:
                    state.mode = 5;
                    break;

                case 1331:
                    state.mode = 6;
                    break;

                case 1933:
                    state.mode = 7;
                    break;


                case 10:
                    state.flip = false;
                    break;

                case 11:
                    state.flip = true;
                    break;


                case 99:
                    state.mini = false;
                    break;

                case 101:
                    state.mini = true;
                    break;


                case 200:
                    state.speed = 0;
                    break;

                case 201:
                    state.speed = 1;
                    break;

                case 202:
                    state.speed = 2;
                    break;

                case 203:
                    state.speed = 3;
                    break;

                case 1334:
                    state.speed = 4;
                    break;
            }
        }


        bool changed =
            !m_fields->hasPreviousState ||
            m_fields->lastMode != state.mode ||
            m_fields->lastFlip != state.flip ||
            m_fields->lastMini != state.mini ||
            m_fields->lastSpeed != state.speed;


        settings->m_startMode = state.mode;
        settings->m_startMini = state.mini;
        settings->m_isFlipped = state.flip;


        switch (state.speed) {

            case 0:
                settings->m_startSpeed = Speed::Slow;
                break;

            case 1:
                settings->m_startSpeed = Speed::Normal;
                break;

            case 2:
                settings->m_startSpeed = Speed::Fast;
                break;

            case 3:
                settings->m_startSpeed = Speed::Faster;
                break;

            case 4:
                settings->m_startSpeed = Speed::Fastest;
                break;
        }


        start->setSettings(settings);


        if (changed && m_fields->hasPreviousState) {

            Notification::create(
                "SmartPos updated"
            )->show();
        }


        m_fields->lastMode = state.mode;
        m_fields->lastFlip = state.flip;
        m_fields->lastMini = state.mini;
        m_fields->lastSpeed = state.speed;

        m_fields->hasPreviousState = true;

        m_fields->pendingStartPos = nullptr;
    }
};