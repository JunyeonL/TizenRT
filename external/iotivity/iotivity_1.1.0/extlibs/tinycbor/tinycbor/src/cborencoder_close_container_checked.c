/****************************************************************************
**
** Copyright (C) 2015 Intel Corporation
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
**
****************************************************************************/

#define _BSD_SOURCE 1
#include "cbor.h"
#include "cborconstants_p.h"
#include "compilersupport_p.h"
#include "extract_number_p.h"

#include <assert.h>

#include "assert_p.h"       /* Always include last */

CborError cbor_encoder_close_container_checked(CborEncoder *encoder, const CborEncoder *containerEncoder)
{
  const uint8_t *ptr = encoder->ptr;
  CborError err = cbor_encoder_close_container(encoder, containerEncoder);
  if (containerEncoder->flags & CborIteratorFlag_UnknownLength || encoder->end == NULL)
    return err;

  // check what the original length was
  uint64_t actually_added;
  err = extract_number(&ptr, encoder->ptr, &actually_added);
  if (err)
    return err;

  if (containerEncoder->flags & CborIteratorFlag_ContainerIsMap)
  {
    if (actually_added > SIZE_MAX / 2)
      return CborErrorDataTooLarge;
    actually_added *= 2;
  }
  return actually_added == containerEncoder->added ? CborNoError :
         actually_added < containerEncoder->added ? CborErrorTooManyItems : CborErrorTooFewItems;
}
