#include "qua_version.h"

#if defined(WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdio.h>

#endif


#include "BaseVal.h"
#include "Stackable.h"
//
// List of TypedValue....
//

//// will need garbage collection ?????????
//TypedValueList::TypedValueList()
//{
//	Count = 0;
//}

void
TypedValueList::Clear()
{
	if (head) {
		delete head;
		head = nullptr;
	}
	tail = nullptr;
}

void
TypedValueList::AddItem(void *I, short t)
{
	TypedValueListItem *ti = new TypedValueListItem();
	ti->value.Set(t, TypedValue::REF_POINTER);
	ti->value.SetValue(I);
	if (head == nullptr || tail == nullptr) {
		head = tail = ti;
	} else {
		tail->next = ti;
		tail = ti;
	}
}

void
TypedValueList::AddItem(TypedValue *val)
{
	TypedValueListItem *ti = new TypedValueListItem();
	ti->value.Set(val);
	if (head == nullptr || tail == nullptr) {
		head = tail = ti;
	} else {
		tail->next = ti;
		tail = ti;
	}
}

#ifdef QUA_V_APP_HANDLER
bool
TypedValueList::AddToMessage(BMessage *msg)
{
#ifdef OLD_LIST
	if (Items[0].type != S_STRING)
		return FALSE;
	for (int i=1; i<Count; i++) {
		switch(Items[i].type) {
		case S_INT:
			msg->AddInt32(Items[0].StringValue(),
						Items[i].IntValue(nullptr));
			break;

		case S_BYTE:
			msg->AddInt8(Items[0].StringValue(),
						Items[i].ByteValue(nullptr));
			break;
		case S_FLOAT:
			msg->AddFloat(Items[0].StringValue(),
						Items[i].FloatValue(nullptr));
			break;

		case S_SHORT:
			msg->AddInt16(Items[0].StringValue(),
						Items[i].ShortValue(nullptr));
			break;

		case S_BOOL:
			msg->AddInt32(Items[0].StringValue(),
						Items[i].BoolValue(nullptr));
			break;

		case S_STRING:
			msg->AddString(Items[0].StringValue(),
						Items[i].StringValue());
			break;
		case S_LONG:
			msg->AddInt64(Items[0].StringValue(),
						Items[i].LongValue(nullptr));
			break;

		case S_LIST	:			// send all values.
			break;
		case S_APPLICATION:		// send messenger
			break;
		
//		case S_UNKNOWN: break;
//		case S_VOICE: break;
//		case S_POOL	: break;
//		case S_METHOD: break;
//		case S_STREAM:	break;
//		case S_CHANNEL:	break;
//		case S_QUA:	break;
//		case S_STREAM_ITEM:	break;
//		case S_VALUE:	break;
//		case S_TIME:	break;
//		case S_TIME:	break;
//		case S_NOTE	:	break;
//		case S_NOTE:	break;
//		case S_CTRL	:	break;
//		case S_CTRL:	break;
//		case S_SYSX	:	break;
//		case S_SYSX:	break;
//		case S_SYSC	:	break;
//		case S_SYSC	:	break;
//		case S_MESSAGE:	break;
//		case S_EXPRESSION:	break;
//		case S_BLOCK:	break;
//		case S_BEND:	break;
//		case S_BEND	:	break;
//		case S_DESTINATION:	break;
		}
	}
#else
	TypedValueListItem	*p = head;
	if (!p)
		return false;
	if (head->value.type != S_STRING)
		return FALSE;
	p = p->next;
	while (p) {
		switch(p->value.type) {
		case S_INT:
			msg->AddInt32(head->value.StringValue(),
						p->value.IntValue(nullptr));
			break;

		case S_BYTE:
			msg->AddInt8(head->value.StringValue(),
						p->value.ByteValue(nullptr));
			break;
		case S_FLOAT:
			msg->AddFloat(head->value.StringValue(),
						p->value.FloatValue(nullptr));
			break;

		case S_SHORT:
			msg->AddInt16(head->value.StringValue(),
						p->value.ShortValue(nullptr));
			break;

		case S_BOOL:
			msg->AddInt32(head->value.StringValue(),
						p->value.BoolValue(nullptr));
			break;

		case S_STRING:
			msg->AddString(head->value.StringValue(),
						p->value.StringValue());
			break;
		case S_LONG:
			msg->AddInt64(head->value.StringValue(),
						p->value.LongValue(nullptr));
			break;

		case S_LIST	:			// send all values.
			break;
		case S_APPLICATION:		// send messenger
			break;
		
//		case S_UNKNOWN: break;
//		case S_VOICE: break;
//		case S_POOL	: break;
//		case S_METHOD: break;
//		case S_STREAM:	break;
//		case S_CHANNEL:	break;
//		case S_QUA:	break;
//		case S_STREAM_ITEM:	break;
//		case S_VALUE:	break;
//		case S_TIME:	break;
//		case S_TIME:	break;
//		case S_NOTE	:	break;
//		case S_NOTE:	break;
//		case S_CTRL	:	break;
//		case S_CTRL:	break;
//		case S_SYSX	:	break;
//		case S_SYSX:	break;
//		case S_SYSC	:	break;
//		case S_SYSC	:	break;
//		case S_MESSAGE:	break;
//		case S_EXPRESSION:	break;
//		case S_BLOCK:	break;
//		case S_BEND:	break;
//		case S_BEND	:	break;
//		case S_DESTINATION:	break;
		}
	}
#endif
	return TRUE;
}

#endif
