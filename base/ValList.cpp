#include "qua_version.h"

#include "BaseVal.h"
#include "Stackable.h"
#include "OSCMessage.h"
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
	ti->value.SetPointerValue(I);
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

bool
TypedValueList::AddToMessage(OSCMessage *msg)
{
	TypedValueListItem	*p = head;
	if (!p)
		return false;
	if (head->value.type != TypedValue::S_STRING)
		return false;
	p = p->next;
	while (p) {
		switch(p->value.type) {
		case TypedValue::S_INT:
			msg->addInt32(p->value.IntValue(nullptr));
			break;

		case TypedValue::S_BYTE:
			msg->addInt8(p->value.ByteValue(nullptr));
			break;
		case TypedValue::S_FLOAT:
			msg->addFloat(p->value.FloatValue(nullptr));
			break;

		case TypedValue::S_SHORT:
			msg->addInt16(p->value.ShortValue(nullptr));
			break;

		case TypedValue::S_BOOL:
			msg->addInt32(p->value.BoolValue(nullptr));
			break;

		case TypedValue::S_STRING:
			msg->addString(p->value.StringValue());
			break;
		case TypedValue::S_LONG:
			msg->addInt64(p->value.LongValue(nullptr));
			break;

		case TypedValue::S_LIST	:			// send all values.
			break;
//		case S_UNKNOWN: break;
//		case S_VOICE: break;
//		case S_POOL	: break;
//		case S_LAMBDA: break;
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
	return true;
}

