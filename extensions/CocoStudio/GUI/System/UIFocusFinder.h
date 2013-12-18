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

#ifndef UIFOCUSFINDER_H_
#define UIFOCUSFINDER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

NS_CC_EXT_BEGIN

typedef enum {
    kFocusUp = 1,
    kFocusDown,
    kFocusLeft,
    kFocusRight,
    kFocusTouch,
    kFocusUnknown
} ccFocusDirection;

class UIFocusFinder : CCObject {
public:
    static UIFocusFinder* sharedFocusFinder();

    UIWidget* focusSearch(UIWidget* pRoot, UIWidget* pFocus, int direction);

private:
    UIFocusFinder();

    ~UIFocusFinder();

    CREATE_FUNC(UIFocusFinder);

    bool init();

    void setThreshold(int xThreshold, int yThreshold);

    UIWidget* search(CCArray* focusables, int direction);

    UIWidget* search(CCArray* focusables, UIWidget* pRoot, UIWidget* pFocus, int direction);

private:
    int m_nThresholdX;
    int m_nThresholdY;
};

NS_CC_EXT_END

#endif /* UIFOCUSFINDER_H_ */
