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

#include <svtools/iconview.hxx>
#include <svtools/treelistentry.hxx>
#include <svtools/fileview.hxx>
#include <iconviewimpl.hxx>

IconView::IconView( vcl::Window* pParent, WinBits nBits )
: SvTreeListBox( pParent, nBits )
{
    nColumns = 1;
    mbCenterAndClipText = true;
    SetEntryHeight( 100 );
    SetEntryWidth( 100 );

    pImpl.reset( new IconViewImpl( this, GetModel(), GetStyle() ) );
}

void IconView::Resize()
{
    Size aBoxSize = Control::GetParent()->GetOutputSizePixel();

    if ( !aBoxSize.Width() )
        return;

    SetSizePixel( aBoxSize );

    nColumns = aBoxSize.Width() / nEntryWidth;

    SvTreeListBox::Resize();
}

tools::Rectangle IconView::GetFocusRect( SvTreeListEntry*, long nEntryPos )
{
    Size aSize;
    aSize.setHeight( nEntryHeight );
    aSize.setWidth( nEntryWidth );

    Point aPos;
    aPos.setX( 0 );
    aPos.setY( 0 );

    tools::Rectangle aRect;

    short nCols = GetColumnsCount();

    if(nCols)
    {
        aPos.setY( ( nEntryPos / nCols ) * nEntryHeight );
        aPos.setX( ( nEntryPos % nCols ) * nEntryWidth );
    }

    aRect.SetPos( aPos );
    aRect.SetSize( aSize );

    return aRect;
}

void IconView::ClearAll()
{
    for ( sal_uLong i = 0; i < GetEntryCount(); ++i )
        delete static_cast<SvtContentEntry*>(GetEntry(i)->GetUserData());

    Clear();
}

void IconView::PaintEntry(SvTreeListEntry& rEntry, long nX, long nY,
                            vcl::RenderContext& rRenderContext)
{

    tools::Rectangle aRect; // multi purpose

    PreparePaint(rRenderContext, rEntry);

    pImpl->UpdateContextBmpWidthMax(&rEntry);

    short nTempEntryHeight = GetEntryHeight();
    short nTempEntryWidth = GetEntryWidth();

    Point aEntryPos;

    Color aBackupTextColor(rRenderContext.GetTextColor());
    vcl::Font aBackupFont(rRenderContext.GetFont());
    Color aBackupColor = rRenderContext.GetFillColor();

    bool bCurFontIsSel = false;
    bool bInUse = rEntry.HasInUseEmphasis();
    const WinBits nWindowStyle = GetStyle();
    const bool bHideSelection = (nWindowStyle & WB_HIDESELECTION) !=0 && !HasFocus();
    const StyleSettings& rSettings = rRenderContext.GetSettings().GetStyleSettings();

    vcl::Font aHighlightFont(rRenderContext.GetFont());
    const Color aHighlightTextColor(rSettings.GetHighlightTextColor());
    aHighlightFont.SetColor(aHighlightTextColor);

    Size aRectSize(nTempEntryWidth, nTempEntryHeight);

    SvViewDataEntry* pViewDataEntry = GetViewDataEntry( &rEntry );

    sal_uInt16 nItemCount = rEntry.ItemCount();
    sal_uInt16 nCurItem = 0;
    sal_uInt16 nIconItem = nItemCount;

    while (nCurItem < nItemCount)
    {
        SvLBoxItem* pItem = nCurItem < nItemCount ? &rEntry.GetItem(nCurItem) : nullptr;
        SvLBoxItemType nItemType = pItem->GetType();

        if (nItemType == SvLBoxItemType::ContextBmp)
        {
            nIconItem = nCurItem;
            nCurItem++;
            continue;
        }

        Size aSize(SvLBoxItem::GetSize(pViewDataEntry, nCurItem));

        aEntryPos.setX( nX );
        aEntryPos.setY( nY );

        // set background pattern/color

        Wallpaper aWallpaper = rRenderContext.GetBackground();

        if (pViewDataEntry->IsHighlighted())
        {
            Color aNewWallColor = rSettings.GetHighlightColor();
            if (!bInUse)
            {
                // if the face color is bright then the deactive color is also bright
                // -> so you can't see any deactive selection
                if (bHideSelection && !rSettings.GetFaceColor().IsBright()
                   && aWallpaper.GetColor().IsBright() != rSettings.GetDeactiveColor().IsBright())
                {
                    aNewWallColor = rSettings.GetDeactiveColor();
                }
                // set font color to highlight
                if (!bCurFontIsSel)
                {
                    rRenderContext.SetTextColor(aHighlightTextColor);
                    rRenderContext.SetFont(aHighlightFont);
                    bCurFontIsSel = true;
                }
            }
            aWallpaper.SetColor(aNewWallColor);
        }
        else  // no selection
        {
            if (bCurFontIsSel)
            {
                bCurFontIsSel = false;
                rRenderContext.SetTextColor(aBackupTextColor);
                rRenderContext.SetFont(aBackupFont);
            }
            else
            {
                aWallpaper.SetColor(rEntry.GetBackColor());
            }
        }

        // draw background
        if (!(nTreeFlags & SvTreeFlags::USESEL))
        {
            aRect.SetPos(aEntryPos);
            aRect.SetSize(aRectSize);

            Color aBackgroundColor = aWallpaper.GetColor();
            if (aBackgroundColor != COL_TRANSPARENT)
            {
                rRenderContext.SetFillColor(aBackgroundColor);
                // this case may occur for smaller horizontal resizes
                if (aRect.Left() < aRect.Right())
                    rRenderContext.DrawRect(aRect);
            }
        }

        // center vertically
        aEntryPos.AdjustY((nTempEntryHeight - aSize.Height()) / 2 );

        // draw item
        pViewDataEntry->SetPaintRectangle(aRect);

        aEntryPos.AdjustY(15 );

        pItem->Paint(aEntryPos, *this, rRenderContext, pViewDataEntry, rEntry);

        rRenderContext.SetFillColor(aBackupColor);

        nCurItem++;
    }

    // draw icon
    if(nIconItem != nItemCount && nIconItem < nItemCount)
    {
        Size aSize(SvLBoxItem::GetSize(pViewDataEntry, nIconItem));

        aEntryPos.setX( nX );
        aEntryPos.setY( nY );

        // center horizontally
        aEntryPos.AdjustX((nTempEntryWidth - aSize.Width()) / 2 );
        // center vertically
        aEntryPos.AdjustY((nTempEntryHeight - aSize.Height()) / 2 );

        aEntryPos.AdjustY( -10 );

        SvLBoxItem* pItem = &rEntry.GetItem(nIconItem);

        pItem->Paint(aEntryPos, *this, rRenderContext, pViewDataEntry, rEntry);
    }

    if (bCurFontIsSel)
    {
        rRenderContext.SetTextColor(aBackupTextColor);
        rRenderContext.SetFont(aBackupFont);
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
