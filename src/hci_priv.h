/**
 * @file hci_priv.h
 * @brief HCI command implementation
 * @author Florin Iucha <florin@signbit.net>
 * @copyright Apache License, Version 2.0
 */

/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is part of LightBLUE Bluetooth Smart Library
 */

#ifndef __HCI_PRIV_H__
#define __HCI_PRIV_H__

/**
 * @privatesection
 */

#include <hci.h>

void hci_initialize(void);

void hci_cleanup(void);

struct hci_condition;

struct hci_condition* allocateCondition(uint16_t opcode, uint8_t* result, uint8_t capacity);

void releaseCondition(uint16_t opcode);

void signalCondition(uint16_t opcode, enum HCI_StatusCode status, const uint8_t* result, uint8_t length);

enum HCI_StatusCode waitForCondition(struct hci_condition* cond, uint8_t* length);

extern unsigned lbDebugLevel;

void hci_on_ATT_READ_BY_GROUP_TYPE_RESP_EVENT(struct LB_Controller* controller, const uint8_t* buffer, uint8_t length);


#endif // __HCI_PRIV_H__

