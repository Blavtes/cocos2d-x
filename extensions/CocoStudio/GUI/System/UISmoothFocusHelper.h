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

#ifndef UISMOOTHFOCUSHELPER_H_
#define UISMOOTHFOCUSHELPER_H_

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class UISmoothFocusHelper;

#define ANIMATION_DURATION 0.08f
#define ANIMATION_START_THRESHOLD 0.001f
#define ANIMATION_END_THRESHOLD 0.999f

#define ANIMATION_START 0
#define ANIMATION_END 1

typedef void (CCObject::*SEL_AnimationEvent)(CCAction*, int);
typedef void (CCObject::*SEL_SmoothFocusEvent)(UISmoothFocusHelper*, int);

#define animation_selector(_SELECTOR) SEL_AnimationEvent(&_SELECTOR)
#define smoothFocus_selector(_SELECTOR) SEL_SmoothFocusEvent(&_SELECTOR)

class UISmoothFocusHelper : public CCObject {
public:
    UISmoothFocusHelper();
    virtual ~UISmoothFocusHelper();

    static UISmoothFocusHelper* create(UIWidget* pRoot, const char* focusIndicator);

    static UISmoothFocusHelper* create(UIWidget* pRoot, CCNode* pIndicator);

    virtual UIWidget* getRootWidget();

    virtual CCNode* getFocusIndicator();

    virtual void setFocusIndicatorZOrder(int zorder);

    virtual void setFocusIndicatorPadding(float paddingX, float paddingY);

    virtual void setAnimationDuration(float duration);

    virtual bool isAnimating();

    virtual void addSmoothFocusEventListener(CCObject* target, SEL_SmoothFocusEvent selector);

protected:
    virtual bool init(UIWidget* pRoot, CCNode* pIndicator);

    virtual void createSmoothFocusActions(CCArray* actions, UIWidget* pCurrFocus);

    virtual bool moveFocus(UIWidget* pPrevFocus, UIWidget* pCurrFocus);

    virtual bool focusIn(UIWidget* pCurrFocus);

    virtual bool focusOut();

    virtual void globalFocusEvent(UIWidget* pPrevFocus, UIWidget* pCurrFocus);

    virtual void animationEvent(CCAction* action, int state);

protected:
    UIWidget* m_pRootWidget;
    CCNode* m_pFocusIndicator;
    UIWidget* m_pSelectedWidget;

    float m_fFocusIndicatorPaddingX;
    float m_fFocusIndicatorPaddingY;

    float m_fAnimationDuration;

    bool m_bIsAnimating;

    CCObject* m_pSmoothFocusEventListener;
    SEL_SmoothFocusEvent m_pfnSmoothFocusEventSelector;
};

class UISmoothFocusSpawn : public CCSpawn {

public:
    static UISmoothFocusSpawn* create(CCArray* actions);

    static UISmoothFocusSpawn* createWithTwoActions(CCFiniteTimeAction *pAction1,
            CCFiniteTimeAction *pAction2);

    virtual void addAnimationEventListener(CCObject* target, SEL_AnimationEvent selector);

    virtual void update(float dt);

private:
    UISmoothFocusSpawn();
    ~UISmoothFocusSpawn();

    CCObject* m_pAnimationEventListener;
    SEL_AnimationEvent m_pfnAnimationEventSelector;
};

// Extra action for making a CCSequence or CCSpawn when only adding one action to it.
class ExtraAction : public CCFiniteTimeAction {
public:
    static ExtraAction* create();

    virtual CCObject* copyWithZone(CCZone* pZone);

    virtual ExtraAction* reverse(void);

    virtual void update(float time);

    virtual void step(float dt);
};

#endif /* UISMOOTHFOCUSHELPER_H_ */
