#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/StartPosObject.hpp>
#include <Geode/binding/LevelSettingsObject.hpp>

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
            // Gamemodes
            id == 12 ||
            id == 13 ||
            id == 47 ||
            id == 111 ||
            id == 660 ||
            id == 745 ||
            id == 1331 ||
            id == 1933 ||

            // Gravity
            id == 10 ||
            id == 11 ||

            // Mini
            id == 99 ||
            id == 101 ||

            // Speed
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

        // Wait until the player stops moving it.
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

        if (newest) {
            // Placement updates immediately.
            scheduleScan(newest, 0.f);
        }
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

    // Distance from StartPos to the closest portal we've found.
    float closestMode = FLT_MAX;
    float closestGravity = FLT_MAX;
    float closestMini = FLT_MAX;
    float closestSpeed = FLT_MAX;

    for (auto obj : CCArrayExt<GameObject*>(objects)) {

        if (!obj)
            continue;

        if (!isPortal(obj->m_objectID))
            continue;

        // Ignore portals after the StartPos.
        if (obj->getPositionX() > startX)
            continue;

        float dist = startX - obj->getPositionX();

        switch (obj->m_objectID) {

        // ---------------- Gamemodes ----------------

        case 12:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 0;
            }
            break;

        case 13:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 1;
            }
            break;

        case 47:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 2;
            }
            break;

        case 111:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 3;
            }
            break;

        case 660:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 4;
            }
            break;

        case 745:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 5;
            }
            break;

        case 1331:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 6;
            }
            break;

        case 1933:
            if (dist < closestMode) {
                closestMode = dist;
                state.mode = 7;
            }
            break;

        // ---------------- Gravity ----------------

        case 10:
            if (dist < closestGravity) {
                closestGravity = dist;
                state.flip = false;
            }
            break;

        case 11:
            if (dist < closestGravity) {
                closestGravity = dist;
                state.flip = true;
            }
            break;

        // ---------------- Mini ----------------

        case 99:
            if (dist < closestMini) {
                closestMini = dist;
                state.mini = false;
            }
            break;

        case 101:
            if (dist < closestMini) {
                closestMini = dist;
                state.mini = true;
            }
            break;

        // ---------------- Speed ----------------

        case 200:
            if (dist < closestSpeed) {
                closestSpeed = dist;
                state.speed = 0;
            }
            break;

        case 201:
            if (dist < closestSpeed) {
                closestSpeed = dist;
                state.speed = 1;
            }
            break;

        case 202:
            if (dist < closestSpeed) {
                closestSpeed = dist;
                state.speed = 2;
            }
            break;

        case 203:
            if (dist < closestSpeed) {
                closestSpeed = dist;
                state.speed = 3;
            }
            break;

        case 1334:
            if (dist < closestSpeed) {
                closestSpeed = dist;
                state.speed = 4;
            }
            break;
        }
    }

        bool changed =
          !m_fields->hasPreviousState ||
          m_fields->lastMode != state.mode ||
          m_fields->lastFlip != state.flip ||
          m_fields->lastMini != state.mini ||
          m_fields->lastSpeed != state.speed;

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

    if (changed && m_fields->hasPreviousState) {
        Notification::create(
            "StartPos changed"
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