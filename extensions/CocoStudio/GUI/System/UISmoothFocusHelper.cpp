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

#include "UISmoothFocusHelper.h"

/**
 * UISmoothFocusHelper
 */
UISmoothFocusHelper::UISmoothFocusHelper():
m_pRootWidget(NULL),
m_pFocusIndicator(NULL),
m_pSelectedWidget(NULL),
m_fFocusIndicatorPaddingX(0.0f),
m_fFocusIndicatorPaddingY(0.0f),
m_fAnimationDuration(ANIMATION_DURATION),
m_bIsAnimating(false),
m_pSmoothFocusEventListener(NULL),
m_pfnSmoothFocusEventSelector(NULL) {

}

UISmoothFocusHelper::~UISmoothFocusHelper() {
    m_pRootWidget = NULL;
    m_pFocusIndicator = NULL;
    m_pSelectedWidget = NULL;
    m_pSmoothFocusEventListener = NULL;
    m_pfnSmoothFocusEventSelector = NULL;
}

UISmoothFocusHelper* UISmoothFocusHelper::create(UIWidget* pRoot, const char* focusIndicator) {
    return create(pRoot, CCSprite::create(focusIndicator));
}

UISmoothFocusHelper* UISmoothFocusHelper::create(UIWidget* pRoot, CCNode* pIndicator) {
    UISmoothFocusHelper* pRet = new UISmoothFocusHelper();
    if (pRet && pRet->init(pRoot, pIndicator)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

UIWidget* UISmoothFocusHelper::getRootWidget() {
    return m_pRootWidget;
}

CCNode* UISmoothFocusHelper::getFocusIndicator() {
    return m_pFocusIndicator;
}

void UISmoothFocusHelper::setFocusIndicatorZOrder(int zorder) {
    CCNode* pIndicator = getFocusIndicator();
    if (pIndicator && pIndicator->getParent()) {
        pIndicator->getParent()->reorderChild(pIndicator, zorder);
    }
}

void UISmoothFocusHelper::setFocusIndicatorPadding(float paddingX, float paddingY) {
    if (paddingX < 0.0f) {
        CCLOGINFO("smooth focus indicator padding x must >= 0.0f");
        paddingX = 0;
    }

    if (paddingY < 0.0f) {
        CCLOGINFO("smooth focus indicator padding y must >= 0.0f");
        paddingY = 0;
    }

    m_fFocusIndicatorPaddingX = paddingX;
    m_fFocusIndicatorPaddingY = paddingY;
}

void UISmoothFocusHelper::setAnimationDuration(float duration) {
    if (duration <= 0.001f) {
        CCLOGINFO("smooth focus animation duration must > 0.001f");
    } else {
        m_fAnimationDuration = duration;
    }
}

bool UISmoothFocusHelper::isAnimating() {
    return m_bIsAnimating;
}

void UISmoothFocusHelper::addSmoothFocusEventListener(CCObject* target,
        SEL_SmoothFocusEvent selector) {
    m_pSmoothFocusEventListener = target;
    m_pfnSmoothFocusEventSelector = selector;
}

bool UISmoothFocusHelper::init(UIWidget* pRoot, CCNode* pIndicator) {
    m_pRootWidget = pRoot;
    m_pFocusIndicator = pIndicator;

    if (m_pRootWidget && m_pFocusIndicator) {
        m_pFocusIndicator->setAnchorPoint(CCPointMake(0.5, 0.5));
        m_pFocusIndicator->setVisible(false);

        m_pRootWidget->getRenderer()->addChild(m_pFocusIndicator);
        m_pRootWidget->getRootWidget()->addGlobalFocusEventListener(this,
                global_focus_selector(UISmoothFocusHelper::globalFocusEvent));
    }

    return true;
}

void UISmoothFocusHelper::createSmoothFocusActions(CCArray* actions,
        UIWidget* pCurrFocus) {
    UIWidget* pParent = getRootWidget();
    CCNode* pIndicator = getFocusIndicator();
    CCRect currFocusRect = pCurrFocus->getDrawingRect(pParent);

    CCMoveTo* move = CCMoveTo::create(m_fAnimationDuration,
            pCurrFocus->getCenterPoint(pParent));
    CCScaleTo* scale = CCScaleTo::create(m_fAnimationDuration,
            (currFocusRect.size.width + m_fFocusIndicatorPaddingX * 2)
                    / pIndicator->getContentSize().width,
            (currFocusRect.size.height + m_fFocusIndicatorPaddingY * 2)
                    / pIndicator->getContentSize().height);
    move->setTarget(pIndicator);
    scale->setTarget(pIndicator);

    actions->addObject(move);
    actions->addObject(scale);
}

bool UISmoothFocusHelper::moveFocus(UIWidget* pPrevFocus, UIWidget* pCurrFocus) {
    UIWidget* pParent = getRootWidget();
    CCNode* pIndicator = getFocusIndicator();
    if (pParent == NULL || pIndicator == NULL) {
        return false;
    }

    m_pSelectedWidget = pCurrFocus;

    pIndicator->setVisible(true);
    pIndicator->stopAllActions();

    CCArray* actions = CCArray::create();
    createSmoothFocusActions(actions, pCurrFocus);

    UISmoothFocusSpawn* anim = UISmoothFocusSpawn::create(actions);
    if (anim == NULL) {
        return false;
    }

    anim->addAnimationEventListener(this, animation_selector(UISmoothFocusHelper::animationEvent));
    pIndicator->runAction(anim);

    return true;
}

bool UISmoothFocusHelper::focusIn(UIWidget* pCurrFocus) {
    // TODO: may be need force move focus to selected child.
    UIWidget* pParent = getRootWidget();
    CCNode* pIndicator = getFocusIndicator();
    if (pParent == NULL || pIndicator == NULL) {
        return false;
    }

    m_pSelectedWidget = pCurrFocus;

    pIndicator->setVisible(true);

    CCRect currFocusRect = pCurrFocus->getDrawingRect(pParent);

    pIndicator->stopAllActions();
    pIndicator->setScaleX(
            (currFocusRect.size.width + m_fFocusIndicatorPaddingX * 2)
                    / pIndicator->getContentSize().width);
    pIndicator->setScaleY(
            (currFocusRect.size.height + m_fFocusIndicatorPaddingY * 2)
                    / pIndicator->getContentSize().height);
    pIndicator->setPosition(pCurrFocus->getCenterPoint(pParent));

    return true;
}

bool UISmoothFocusHelper::focusOut() {
    CCNode* pIndicator = getFocusIndicator();
    if (pIndicator) {
        pIndicator->setVisible(false);
        pIndicator->stopAllActions();
    }

    return true;
}

void UISmoothFocusHelper::globalFocusEvent(UIWidget* pPrevFocus, UIWidget* pCurrFocus) {
    bool prevIsMyChild = m_pRootWidget->isMyChild(pPrevFocus);
    bool currIsMyChild = m_pRootWidget->isMyChild(pCurrFocus);

    if (prevIsMyChild && currIsMyChild) { // focus moved from one child to another
        m_bIsAnimating = moveFocus(pPrevFocus, pCurrFocus);
        return;
    }

    if (!prevIsMyChild && currIsMyChild) { // focus in
        focusIn(pCurrFocus);
        m_bIsAnimating = false;
        return;
    }

    if (prevIsMyChild && !currIsMyChild) { // focus out
        focusOut();
        m_bIsAnimating = false;
        return;
    }
}

void UISmoothFocusHelper::animationEvent(CCAction* action, int state) {
    m_bIsAnimating = (state != ANIMATION_END);
    if (m_pSmoothFocusEventListener && m_pfnSmoothFocusEventSelector) {
        (m_pSmoothFocusEventListener->*m_pfnSmoothFocusEventSelector)(this, state);
    }
}


/**
 * SmoothFocusSpawn
 */
UISmoothFocusSpawn::UISmoothFocusSpawn():
m_pAnimationEventListener(NULL),
m_pfnAnimationEventSelector(NULL) {

}

UISmoothFocusSpawn::~UISmoothFocusSpawn() {
    m_pAnimationEventListener = NULL;
    m_pfnAnimationEventSelector = NULL;
}

UISmoothFocusSpawn* UISmoothFocusSpawn::create(CCArray *actions) {
    UISmoothFocusSpawn* pRet = NULL;
    do {
        unsigned int count = actions->count();
        CC_BREAK_IF(count == 0);
        CCFiniteTimeAction* prev = (CCFiniteTimeAction*) actions->objectAtIndex(0);
        if (count > 1) {
            for (unsigned int i = 1; i < actions->count(); ++i) {
                prev = createWithTwoActions(prev, (CCFiniteTimeAction*) actions->objectAtIndex(i));
            }
        } else {
            prev = createWithTwoActions(prev, ExtraAction::create());
        }

        pRet = (UISmoothFocusSpawn*) prev;
    } while (0);

    return pRet;
}

UISmoothFocusSpawn* UISmoothFocusSpawn::createWithTwoActions(CCFiniteTimeAction *pAction1,
        CCFiniteTimeAction *pAction2) {
    UISmoothFocusSpawn *pAnimation = new UISmoothFocusSpawn();
    pAnimation->initWithTwoActions(pAction1, pAction2);
    pAnimation->autorelease();

    return pAnimation;
}

void UISmoothFocusSpawn::update(float dt) {
    if (dt <= ANIMATION_START_THRESHOLD) { // animation start
        if (m_pAnimationEventListener && m_pfnAnimationEventSelector) {
            (m_pAnimationEventListener->*m_pfnAnimationEventSelector)(this, ANIMATION_START);
        }

        CCSpawn::update(dt);
    } else if (dt >= ANIMATION_END_THRESHOLD) { // animation end
        CCSpawn::update(dt);

        if (m_pAnimationEventListener && m_pfnAnimationEventSelector) {
            (m_pAnimationEventListener->*m_pfnAnimationEventSelector)(this, ANIMATION_END);
        }
    } else {
        CCSpawn::update(dt);
    }
}

void UISmoothFocusSpawn::addAnimationEventListener(CCObject* target, SEL_AnimationEvent selector) {
    m_pAnimationEventListener = target;
    m_pfnAnimationEventSelector = selector;
}

/**
 * ExtraAction
 */
ExtraAction* ExtraAction::create() {
    ExtraAction* pRet = new ExtraAction();
    if (pRet) {
        pRet->autorelease();
    }
    return pRet;
}

CCObject* ExtraAction::copyWithZone(CCZone* pZone) {
    CC_UNUSED_PARAM(pZone);
    ExtraAction* pRet = new ExtraAction();
    return pRet;
}

ExtraAction* ExtraAction::reverse(void) {
    return ExtraAction::create();
}

void ExtraAction::update(float time) {
    CC_UNUSED_PARAM(time);
}

void ExtraAction::step(float dt) {
    CC_UNUSED_PARAM(dt);
}
