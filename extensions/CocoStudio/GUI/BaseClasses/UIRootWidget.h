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

#ifndef __UIROOTWIDGET_H__
#define __UIROOTWIDGET_H__

#include "../Layouts/Layout.h"

NS_CC_EXT_BEGIN

class UIWidget;

typedef void (CCObject::*SEL_GlobalFocusEvent)(UIWidget*, UIWidget*);
#define global_focus_selector(_SELECTOR) (SEL_GlobalFocusEvent)(&_SELECTOR)

class UIGlobalFocusEventHandler : public CCObject {
public:
    UIGlobalFocusEventHandler(CCObject* target, SEL_GlobalFocusEvent selector);

    ~UIGlobalFocusEventHandler();

    static UIGlobalFocusEventHandler* create(CCObject* target, SEL_GlobalFocusEvent selector);

    virtual bool init();

public:
    CCObject*       m_pGlobalFocusEventListener;
    SEL_GlobalFocusEvent  m_pfnGlobalFocusEventSelector;
};

/**
*   @js NA
*   @lua NA
*/
class UIRootWidget : public Layout
{
public:
    /**
     * Default constructor
     */
    UIRootWidget();
    
    /**
     * Default destructor
     */
    virtual ~UIRootWidget();
    
    /**
     * Allocates and initializes a widget.
     */
    static UIRootWidget* create();
    
    /**
     * Returns the "class name" of widget.
     */
    virtual const char* getDescription() const;

    /**
    * Returns the current focused widget.
    */
    virtual UIWidget* findFocus();

    /**
    * set the current focused widget.
    */
    virtual void setFocus(UIWidget* pFocus, int direction);

    /**
     * add the global focus event target/selector.
     */
    void addGlobalFocusEventListener(CCObject* target, SEL_GlobalFocusEvent selector);

    /**
     * remove the global focus event target/selector.
     */
    void removeGlobalFocusEventListener(CCObject* target);

protected:
    //initializes state of widget.
    virtual bool init();

protected:
    CCArray* m_pGlobalFocusEventHandlers;

    UIWidget* m_pPrevFocus;
    UIWidget* m_pCurrFocus;
};

NS_CC_EXT_END

#endif /* defined(__CocoGUI__RootWidget__) */
