/*
 ==============================================================================
 
 This file is part of the UGEN++ library
 Copyright 2008-11 The University of the West of England.
 by Martin Robinson
 
 ------------------------------------------------------------------------------
 
 UGEN++ can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 UGEN++ is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with UGEN++; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 Boston, MA 02111-1307 USA
 
 The idea for this project and code in the UGen implementations is
 derived from SuperCollider which is also released under the 
 GNU General Public License:
 
 SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
 http://www.audiosynth.com
 
 ==============================================================================
 */

#include "../core/ugen_StandardHeader.h"

#if defined(UGEN_IPHONE)

BEGIN_UGEN_NAMESPACE

#include "ugen_UISlider.h"

END_UGEN_NAMESPACE

#ifdef UGEN_NAMESPACE
using namespace UGEN_NAMESPACE;
#endif


@interface UISliderValueInternalPeer : NSObject
{
@public
	UISliderValueInternal* owner_;
}

- (void) setOwner: (UISliderValueInternal*) owner;
- (void) updateValue: (UISlider *) slider;
@end


@implementation UISliderValueInternalPeer
- (void) setOwner: (UISliderValueInternal*) owner
{
	ugen_assert(owner != 0);
	owner_ = owner;
}

- (void) updateValue: (UISlider *) slider
{
	owner_->setValue([slider value]);
}
@end


	
BEGIN_UGEN_NAMESPACE

UISliderValueInternal::UISliderValueInternal(UISlider* slider) throw()
:	ValueInternal([slider value]),
	slider_(slider)
{
	UISliderValueInternalPeer* peer = [UISliderValueInternalPeer alloc];
	[peer setOwner: this];
	
	[slider_ retain];
	[slider_ addTarget:peer action:@selector(updateValue:) forControlEvents:UIControlEventValueChanged];
		
	voidPeer = (void*)peer;
}

UISliderValueInternal::~UISliderValueInternal() throw()
{
	UISliderValueInternalPeer* peer = (UISliderValueInternalPeer*)voidPeer;
	
	[slider_ removeTarget:peer action:@selector(updateValue:) forControlEvents:UIControlEventValueChanged];
	[slider_ release];
	[peer release];
}

UISliderValue::UISliderValue(UISlider* slider) throw()
:	Value(new UISliderValueInternal(slider))
{
}


END_UGEN_NAMESPACE

#endif
