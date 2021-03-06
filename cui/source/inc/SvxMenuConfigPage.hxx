/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#ifndef INCLUDED_CUI_SOURCE_INC_SVXMENUCONFIGPAGE_HXX
#define INCLUDED_CUI_SOURCE_INC_SVXMENUCONFIGPAGE_HXX

#include <vcl/fixed.hxx>
#include <vcl/layout.hxx>
#include <vcl/lstbox.hxx>
#include <vcl/menubtn.hxx>
#include <vcl/toolbox.hxx>
#include <svtools/treelistbox.hxx>
#include <svtools/svmedit2.hxx>
#include <svtools/svmedit.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XIndexContainer.hpp>
#include <com/sun/star/container/XNameAccess.hpp>
#include <com/sun/star/ui/XUIConfigurationListener.hpp>
#include <com/sun/star/ui/XUIConfigurationManager.hpp>
#include <com/sun/star/ui/XImageManager.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>

#include <sfx2/tabdlg.hxx>
#include <memory>
#include <vector>

#include "cfgutil.hxx"
#include "cfg.hxx" //for SvxConfigPage and SaveInData

class SvxMenuConfigPage : public SvxConfigPage
{
private:
    bool m_bIsMenuBar;
    DECL_LINK( SelectMenu, ListBox&, void );
    DECL_LINK( SelectMenuEntry, SvTreeListBox *, void );

    DECL_LINK( GearHdl, MenuButton *, void );

    DECL_LINK( SelectCategory, ListBox&, void );

    DECL_LINK( AddCommandHdl, Button *, void );
    DECL_LINK( RemoveCommandHdl, Button *, void );

    DECL_LINK( InsertHdl, MenuButton *, void );
    DECL_LINK( ModifyItemHdl, MenuButton *, void );
    DECL_LINK( ResetMenuHdl, Button *, void );

    void            Init() override;
    void            UpdateButtonStates() override;
    short           QueryReset() override;
    void            DeleteSelectedContent() override;
    void            DeleteSelectedTopLevel() override;

public:
    SvxMenuConfigPage( vcl::Window *pParent, const SfxItemSet& rItemSet, bool bIsMenuBar = true );
    virtual ~SvxMenuConfigPage() override;
    virtual void dispose() override;

    SaveInData* CreateSaveInData(
        const css::uno::Reference <
            css::ui::XUIConfigurationManager >&,
        const css::uno::Reference <
            css::ui::XUIConfigurationManager >&,
        const OUString& aModuleId,
        bool docConfig ) override;
};

#endif // INCLUDED_CUI_SOURCE_INC_SVXMENUCONFIGPAGE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
