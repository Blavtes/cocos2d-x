/**
 * Copyright (C) 2013 Togic Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "UIFocusFinder.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define CCLogPoint(str, p) CCLog("%s, CCPoint = %f ,%f",(const char *) (str), p.x, p.y);

static UIFocusFinder* s_SharedFinder = NULL;

UIFocusFinder* UIFocusFinder::sharedFocusFinder(void) {
    if (!s_SharedFinder) {
        s_SharedFinder = new UIFocusFinder();
        CCAssert(s_SharedFinder, "$$$$$$$$$ can not create FocusFinder.");
    }

    return s_SharedFinder;
}

UIFocusFinder::UIFocusFinder() {
    setThreshold(0, 0);
}

UIFocusFinder::~UIFocusFinder() {

}

bool UIFocusFinder::init() {
    return true;
}

void UIFocusFinder::setThreshold(int xThreshold, int yThreshold) {
    CCLog("$$$$$ setThreshold = %d, %d", xThreshold, yThreshold);
    m_nThresholdX = xThreshold;
    m_nThresholdY = yThreshold;
}

UIWidget* UIFocusFinder::search(CCArray* focusables, int direction) {
    UIWidget* widget = NULL;
    CCObject* tmp = NULL;
    switch (direction) {
    case kFocusUp:
    case kFocusLeft:
        CCARRAY_FOREACH_REVERSE(focusables, tmp) {
            widget = dynamic_cast<UIWidget*>(tmp);
            if (widget) {
                return widget;
            }
        }
        break;
    case kFocusDown:
    case kFocusRight:
        CCARRAY_FOREACH(focusables, tmp) {
            widget = dynamic_cast<UIWidget*>(tmp);
            if (widget) {
                return widget;
            }
        }
        break;
    }

    return NULL;
}

UIWidget* UIFocusFinder::search(CCArray* focusables, UIWidget* pRoot, UIWidget* pFocus,
        int direction) {
    CCPoint start = pFocus->getCenterPoint(pRoot);
    CCLogPoint("search focus from ", start);
    UIWidget* candidate = NULL;
    UIWidget* tmpWidget = NULL;
    CCObject* tmpObj = NULL;
    CCPoint tmpPoint = CCPointZero;
    int minDistance = INT_MAX;
    int tmpDistance = 0;
    switch (direction) {
    case kFocusUp:
        start = start + CCPointMake(0, 1);
        CCARRAY_FOREACH(focusables, tmpObj) {
            tmpWidget = (UIWidget*) tmpObj;
            tmpPoint = tmpWidget->getCenterPoint(pRoot);
            CCLogPoint("&&&&&&& kFocusUp: ", tmpPoint);
            if (tmpPoint.y <= start.y) {
                continue;
            } else if (fabsf(tmpPoint.x - start.x) > m_nThresholdX) {
                continue;
            }

            tmpDistance = start.getDistance(tmpPoint);
            CCLog("&&&&&&& kFocusUp distance: %d", tmpDistance);
            if (tmpDistance < minDistance) {
                minDistance = tmpDistance;
                candidate = tmpWidget;
            }
        }
        break;
    case kFocusDown:
        start = start + CCPointMake(0, -1);
        CCARRAY_FOREACH(focusables, tmpObj) {
            tmpWidget = (UIWidget*) tmpObj;
            tmpPoint = tmpWidget->getCenterPoint(pRoot);
            CCLogPoint("&&&&&&& kFocusDown: ", tmpPoint);

            if (tmpPoint.y >= start.y) {
                continue;
            } else if (fabsf(tmpPoint.x - start.x) > m_nThresholdX) {
                continue;
            }

            tmpDistance = start.getDistance(tmpPoint);
            CCLog("&&&&&&& kFocusDown distance: %d", tmpDistance);
            if (tmpDistance < minDistance) {
                minDistance = tmpDistance;
                candidate = tmpWidget;
            }
        }
        break;
    case kFocusLeft:
        start = start + CCPointMake(-1, 0);
        CCARRAY_FOREACH(focusables, tmpObj) {
            tmpWidget = (UIWidget*) tmpObj;
            tmpPoint = tmpWidget->getCenterPoint(pRoot);
            CCLogPoint("&&&&&&& kFocusLeft: ", tmpPoint);

            if (tmpPoint.x >= start.x) {
                continue;
            } else if (fabsf(tmpPoint.y - start.y) > m_nThresholdY) {
                continue;
            }

            tmpDistance = start.getDistance(tmpPoint);
            CCLog("&&&&&&& kFocusLeft distance: %d", tmpDistance);
            if (tmpDistance < minDistance) {
                minDistance = tmpDistance;
                candidate = tmpWidget;
            }
        }
        break;
    case kFocusRight:
        start = start + CCPointMake(1, 0);
        CCARRAY_FOREACH(focusables, tmpObj) {
            tmpWidget = (UIWidget*) tmpObj;
            tmpPoint = tmpWidget->getCenterPoint(pRoot);
            CCLogPoint("&&&&&&& kFocusRight: ", tmpPoint);

            if (tmpPoint.x <= start.x) {
                continue;
            } else if (fabsf(tmpPoint.y - start.y) > m_nThresholdY) {
                continue;
            }

            tmpDistance = start.getDistance(tmpPoint);
            CCLog("&&&&&&& kFocusRight distance: %d", tmpDistance);
            if (tmpDistance < minDistance) {
                minDistance = tmpDistance;
                candidate = tmpWidget;
            }
        }
        break;
    }

    return candidate;
}

UIWidget* UIFocusFinder::focusSearch(UIWidget* pRoot, UIWidget* pFocus, int direction) {
    if (!pRoot || !pRoot->getRootWidget()) {
        CCLog("can't find focus in a invalid root UIWidget.");
        return NULL;
    }

    if (pRoot == pFocus) {
        CCLog("focused widget is equals root widget, so do nothing.");
        return NULL;
    }

    if (pFocus) {
        CCLog("$$$$$ focusSearch in %s from %s by direction %d", pRoot->getDescription(),
                pFocus->getDescription(),
                direction);
    } else {
        CCLog("$$$$$ focusSearch in %s from %ld by direction %d", pRoot->getDescription(),
                long(pFocus), direction);
    }

    CCArray* focusables = CCArray::create();
    focusables->retain();
    pRoot->getFocusables(focusables);

    if (pFocus) {
        setThreshold((int) (pFocus->getSize().width),
                (int) (pFocus->getSize().height));
    } else {
        setThreshold(100, 100);
    }

    UIWidget* widget = NULL;
    for (int i = focusables->count() - 1; i >= 0; i--) {
        widget = dynamic_cast<UIWidget*>(focusables->objectAtIndex(i));
        if (!widget || !widget->isFocusEnabled()) {
            focusables->removeObjectAtIndex(i, true);
        } else if (widget == pFocus || widget->isMyChild(pFocus)) {
            focusables->removeObjectAtIndex(i, true);
        }
    }

    CCLog("focusable widget count = %d", focusables->count());
    if (focusables->count() <= 0) {
        return NULL;
    }

    UIWidget* candidateFocus = NULL;
    if (!pRoot->isMyChild(pFocus)) {
        CCLog("find the first or last widget in root and gain focus.");
        candidateFocus = search(focusables, direction);
    } else {
        CCLog("find the nearest widget in root and gain focus.");
        candidateFocus = search(focusables, pRoot, pFocus, direction);
    }

    focusables->removeAllObjects();
    focusables->release();

    if (candidateFocus) {
        CCLog("$$$$$$$$$$ find next focus %s in %s.", pRoot->getDescription(),
                candidateFocus->getDescription());
        pRoot->getRootWidget()->setFocus(candidateFocus->gainFocus(direction), direction);
        return candidateFocus;
    } else {
        CCLog("$$$$$$$$$$$$ can't find next focus in %s", pRoot->getDescription());
        return NULL;
    }
}
