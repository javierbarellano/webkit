/*
 * HNEventError.h
 *
 *  Created on: Feb 22, 2012
 *      Author: gar
 */

#ifndef HNEVENTERROR_H_
#define HNEVENTERROR_H_

namespace WebCore {

class HNEventError
{
public:
	HNEventError(int err) { err_ = err; }

	int getErr() { return err_; }

private:
	int err_;

};

} // namespace WebKit

#endif /* HNEVENTERROR_H_ */
