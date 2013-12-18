/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "UIRootWidget.h"

NS_CC_EXT_BEGIN

UIGlobalFocusEventHandler::UIGlobalFocusEventHandler(CCObject* target, SEL_GlobalFocusEvent selector):
m_pGlobalFocusEventListener(target),
m_pfnGlobalFocusEventSelector(selector) {
}

UIGlobalFocusEventHandler::~UIGlobalFocusEventHandler() {
    m_pGlobalFocusEventListener = NULL;
    m_pfnGlobalFocusEventSelector = NULL;
}

UIGlobalFocusEventHandler* UIGlobalFocusEventHandler::create(CCObject* target, SEL_GlobalFocusEvent selector) {
    UIGlobalFocusEventHandler* handler = new UIGlobalFocusEventHandler(target, selector);
    if (handler && handler->init()) {
        handler->autorelease();
        return handler;
    }
    CC_SAFE_DELETE(handler);
    return NULL;
}

bool UIGlobalFocusEventHandler::init() {
    return true;
}



UIRootWidget::UIRootWidget():
m_pGlobalFocusEventHandlers(NULL),
m_pPrevFocus(NULL),
m_pCurrFocus(NULL)
{
    m_pGlobalFocusEventHandlers = CCArray::create();
    m_pGlobalFocusEventHandlers->retain();
}

UIRootWidget::~UIRootWidget()
{
    m_pGlobalFocusEventHandlers->removeAllObjects();
    CC_SAFE_RELEASE_NULL(m_pGlobalFocusEventHandlers);

    m_pPrevFocus = NULL;
    m_pCurrFocus = NULL;
}

UIRootWidget* UIRootWidget::create()
{
    UIRootWidget* widget = new UIRootWidget();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return NULL;
}

bool UIRootWidget::init()
{
    if (Layout::init())
    {
        setSize(CCDirector::sharedDirector()->getWinSize());
        return true;
    }
    return false;
}

const char* UIRootWidget::getDescription() const
{
    return "RootWidget";
}

UIWidget* UIRootWidget::findFocus() {
    return m_pCurrFocus;
}

void UIRootWidget::setFocus(UIWidget* pFocus, int direction) {
    if (pFocus == m_pCurrFocus) {
        return;
    }

    if (m_pCurrFocus) {
        m_pCurrFocus->clearFocus(direction);
    }

    m_pPrevFocus = m_pCurrFocus;
    m_pCurrFocus = pFocus;

    CCObject* tmp = NULL;
    UIGlobalFocusEventHandler* h = NULL;
    CCARRAY_FOREACH(m_pGlobalFocusEventHandlers, tmp) {
        h = dynamic_cast<UIGlobalFocusEventHandler*>(tmp);
        if (h) {
            (h->m_pGlobalFocusEventListener->*(h->m_pfnGlobalFocusEventSelector))(m_pPrevFocus, m_pCurrFocus);
        }
    }
}

void UIRootWidget::addGlobalFocusEventListener(CCObject* target, SEL_GlobalFocusEvent selector) {
    if (!target || !selector) {
        return;
    }

    CCObject* tmp = NULL;
    UIGlobalFocusEventHandler* h = NULL;
    CCARRAY_FOREACH(m_pGlobalFocusEventHandlers, tmp) {
        h = static_cast<UIGlobalFocusEventHandler*>(tmp);
        if (h && h->m_pGlobalFocusEventListener == target) {
            return;
        }
    }

    m_pGlobalFocusEventHandlers->addObject(UIGlobalFocusEventHandler::create(target, selector));
}

void UIRootWidget::removeGlobalFocusEventListener(CCObject* target) {
    if (!target) {
        return;
    }

    UIGlobalFocusEventHandler* tmp = NULL;
    for (int i = m_pGlobalFocusEventHandlers->data->num - 1; i >= 0; i--) {
        tmp = static_cast<UIGlobalFocusEventHandler*>(m_pGlobalFocusEventHandlers->objectAtIndex(i));
        if (tmp && tmp->m_pGlobalFocusEventListener == target) {
            m_pGlobalFocusEventHandlers->removeObjectAtIndex(i, true);
        }
    }
}

NS_CC_EXT_END
