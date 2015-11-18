/*
 * Copyright [2012-2015] DaSE@ECNU
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * error_define.h
 *
 *  Created on: Aug 5, 2015
 *      Author: yukai
 *		 Email: yukai2014@gmail.com
 *
 * Description:
 *
 */

#ifndef COMMON_ERROR_DEFINE_H_
#define COMMON_ERROR_DEFINE_H_
#include <stdlib.h>
#include "./error_no.h"
using claims::common::CStrError;

#define ELOG(ret, err_info)                                                \
  LOG(ERROR) << "[ " << ret << ", " << CStrError(ret) << " ] " << err_info \
             << std::endl;

#define WLOG(ret, warning_info)                                  \
  LOG(WARNING) << "[ " << ret << ", " << CStrError(ret) << " ] " \
               << warning_info << std::endl;

#define EXEC_AND_ONLY_LOG_ERROR(ret, f, err_info) \
  do {                                            \
    if (rSuccess != (ret = f)) {                  \
      ELOG(ret, err_info)                         \
    }                                             \
  } while (0)

#define EXEC_AND_LOG(ret, f, info, err_info) \
  do {                                       \
    if (rSuccess == (ret = f)) {             \
      LOG(INFO) << info << std::endl;        \
    } else {                                 \
      ELOG(ret, err_info)                    \
    }                                        \
  } while (0)

#define EXEC_AND_PLOG(ret, f, info, err_info)                       \
  do {                                                              \
    if (rSuccess == (ret = f)) {                                    \
      LOG(INFO) << info << std::endl;                               \
    } else {                                                        \
      PLOG(ERROR) << "[ " << ret << ", " << CStrError(ret) << " ] " \
                  << err_info << std::endl;                         \
    }                                                               \
  } while (0)

/*
 * have to add Error Message in error_no.h and error_no.cpp, if new error no is
 * add
 */
namespace claims {
namespace common {

const int kSuccess = EXIT_SUCCESS;
const int kFailure = EXIT_FAILURE;

/* errorno begin from here   */

/* errorno for common  -1 ~ -1000 */
const int kTypeError = -1;
const int kNotInit = -2;

/* errorno for SQL parser -1001 ~ -2000  */
const int kNoTableFound = -1001;

/* errorno for Loader -2001 ~ -3000  */
const int rSuccess = -2002;
const int rTooLargeData = -2003;
const int rTooSmallData = -2004;
const int rTooLongData = -2005;
const int rInterruptedData = -2006;  // 123a45 => 123
const int rIncorrectData = -2007;    // a
const int rInvalidNullData = -2008;

/* errorno for codegen -3001 ~ -4000 */
const int kTestError = -3001;

/* errorno for logical_operator -4001 ~ -5000*/
const int kUninitializedJoinPolicy = -4001;

const int kGeneratePlanContextFailed = -4002;

const int kGenerateDataflowFailed = -4003;

/* errorno for physical_operator -5001 ~ -6000 */
const int kGenerateSubPhyPlanFailed = -5001;
const int kNoPartitionIdScan = -5002;
// the optimization of filter function failed;
const int kCodegenFailed = -5003;

}  // end namespace common
}  // end namespace claims

#endif  // COMMON_ERROR_DEFINE_H_
