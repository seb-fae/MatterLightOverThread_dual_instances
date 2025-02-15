/*
 *  Copyright (c) 2016, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file includes definitions for IPv6 sockets.
 */

#ifndef NET_SOCKET_HPP_
#define NET_SOCKET_HPP_

#include "openthread-core-config.h"

#include "common/clearable.hpp"
#include "common/equatable.hpp"
#include "net/ip6_address.hpp"
#include "net/ip6_types.hpp"

namespace ot {

class ThreadLinkInfo;

namespace Ip6 {

/**
 * @addtogroup core-ip6-ip6
 *
 * @{
 */

/**
 * Implements message information for an IPv6 message.
 */
class MessageInfo : public otMessageInfo, public Clearable<MessageInfo>
{
public:
    /**
     * Initializes the object.
     */
    MessageInfo(void) { Clear(); }

    /**
     * Returns a reference to the local socket address.
     *
     * @returns A reference to the local socket address.
     */
    Address &GetSockAddr(void) { return AsCoreType(&mSockAddr); }

    /**
     * Returns a reference to the local socket address.
     *
     * @returns A reference to the local socket address.
     */
    const Address &GetSockAddr(void) const { return AsCoreType(&mSockAddr); }

    /**
     * Sets the local socket address.
     *
     * @param[in]  aAddress  The IPv6 address.
     */
    void SetSockAddr(const Address &aAddress) { mSockAddr = aAddress; }

    /**
     * Gets the local socket port.
     *
     * @returns The local socket port.
     */
    uint16_t GetSockPort(void) const { return mSockPort; }

    /**
     * Gets the local socket port.
     *
     * @param[in]  aPort  The port value.
     */
    void SetSockPort(uint16_t aPort) { mSockPort = aPort; }

    /**
     * Returns a reference to the peer socket address.
     *
     * @returns A reference to the peer socket address.
     */
    Address &GetPeerAddr(void) { return AsCoreType(&mPeerAddr); }

    /**
     * Returns a reference to the peer socket address.
     *
     * @returns A reference to the peer socket address.
     */
    const Address &GetPeerAddr(void) const { return AsCoreType(&mPeerAddr); }

    /**
     * Sets the peer's socket address.
     *
     * @param[in]  aAddress  The IPv6 address.
     */
    void SetPeerAddr(const Address &aAddress) { mPeerAddr = aAddress; }

    /**
     * Gets the peer socket port.
     *
     * @returns The peer socket port.
     */
    uint16_t GetPeerPort(void) const { return mPeerPort; }

    /**
     * Gets the peer socket port.
     *
     * @param[in]  aPort  The port value.
     */
    void SetPeerPort(uint16_t aPort) { mPeerPort = aPort; }

    /**
     * Gets the Hop Limit.
     *
     * @returns The Hop Limit.
     */
    uint8_t GetHopLimit(void) const { return mHopLimit; }

    /**
     * Sets the Hop Limit.
     *
     * @param[in]  aHopLimit  The Hop Limit.
     */
    void SetHopLimit(uint8_t aHopLimit) { mHopLimit = aHopLimit; }

    /**
     * Returns whether multicast may be looped back.
     *
     * @retval TRUE   If message may be looped back.
     * @retval FALSE  If message must not be looped back.
     */
    bool GetMulticastLoop(void) const { return mMulticastLoop; }

    /**
     * Sets whether multicast may be looped back.
     *
     * @param[in]  aMulticastLoop  Whether allow looping back multicast.
     */
    void SetMulticastLoop(bool aMulticastLoop) { mMulticastLoop = aMulticastLoop; }

    /**
     * Gets the ECN status.
     *
     * @returns The ECN status, as represented in the IP header.
     */
    Ecn GetEcn(void) const { return static_cast<Ecn>(mEcn); }

    /**
     * Sets the ECN status.
     *
     * @param[in]  aEcn  The ECN status, as represented in the IP header.
     */
    void SetEcn(Ecn aEcn) { mEcn = aEcn; }

    /**
     * Indicates whether peer is via the host interface.
     *
     * @retval TRUE if the peer is via the host interface.
     * @retval FALSE if the peer is via the Thread interface.
     */
    bool IsHostInterface(void) const { return mIsHostInterface; }

    /**
     * Indicates whether or not to apply hop limit 0.
     *
     * @retval TRUE  if applying hop limit 0 when `mHopLimit` field is 0.
     * @retval FALSE if applying default `OPENTHREAD_CONFIG_IP6_HOP_LIMIT_DEFAULT` when `mHopLimit` field is 0.
     */
    bool ShouldAllowZeroHopLimit(void) const { return mAllowZeroHopLimit; }

    /**
     * Sets whether the peer is via the host interface.
     *
     * @param[in]  aIsHost  TRUE if the peer is via the host interface, FALSE otherwise.
     */
    void SetIsHostInterface(bool aIsHost) { mIsHostInterface = aIsHost; }

    /**
     * Checks if the peer address and port match those of another `MessageInfo`.
     *
     * @param[in] aOther  The other `MessageInfo` to compare with.
     *
     * @retval TRUE   The peer address and port of the two `MessageInfo` objects match.
     * @retval FALSE  The peer address and port of the two `MessageInfo` objects do not match.
     */
    bool HasSamePeerAddrAndPort(const MessageInfo &aOther) const;
};

/**
 * Implements a socket address.
 */
class SockAddr : public otSockAddr, public Clearable<SockAddr>, public Unequatable<SockAddr>
{
public:
    static constexpr uint16_t kInfoStringSize = OT_IP6_SOCK_ADDR_STRING_SIZE; ///< Info string size (`ToString()`).

    /**
     * Defines the fixed-length `String` object returned from `ToString()`.
     */
    typedef String<kInfoStringSize> InfoString;

    /**
     * Initializes the socket address (all fields are set to zero).
     */
    SockAddr(void) { Clear(); }

    /**
     * Initializes the socket address with a given port number.
     *
     * @param[in] aPort   A port number.
     */
    explicit SockAddr(uint16_t aPort)
    {
        mPort = aPort;
        GetAddress().Clear();
    }

    /**
     * Initializes the socket address with a given address and port number.
     *
     * @param[in] aAddress  An IPv6 address.
     * @param[in] aPort     A port number.
     */
    SockAddr(const Address &aAddress, uint16_t aPort)
    {
        mAddress = aAddress;
        mPort    = aPort;
    }

    /**
     * Returns a reference to the IPv6 address.
     *
     * @returns A reference to the IPv6 address.
     */
    Address &GetAddress(void) { return AsCoreType(&mAddress); }

    /**
     * Returns a reference to the IPv6 address.
     *
     * @returns A reference to the IPv6 address.
     */
    const Address &GetAddress(void) const { return AsCoreType(&mAddress); }

    /**
     * Sets the IPv6 address.
     *
     * @param[in] aAddress The IPv6 address.
     */
    void SetAddress(const Address &aAddress) { mAddress = aAddress; }

    /**
     * Returns the socket address port number.
     *
     * @returns The port number
     */
    uint16_t GetPort(void) const { return mPort; }

    /**
     * Sets the socket address port number.
     *
     * @param[in] aPort  The port number.
     */
    void SetPort(uint16_t aPort) { mPort = aPort; }

    /**
     * Overloads operator `==` to evaluate whether or not two `SockAddr` instances are equal (same address
     * and port number).
     *
     * @param[in]  aOther  The other `SockAddr` instance to compare with.
     *
     * @retval TRUE   If the two `SockAddr` instances are equal.
     * @retval FALSE  If the two `SockAddr` instances not equal.
     */
    bool operator==(const SockAddr &aOther) const
    {
        return (GetPort() == aOther.GetPort()) && (GetAddress() == aOther.GetAddress());
    }

    /**
     * Converts the socket address to a string.
     *
     * The string is formatted as "[<ipv6 address>]:<port number>".
     *
     * @returns An `InfoString` containing the string representation of the `SockAddr`
     */
    InfoString ToString(void) const;

    /**
     * Converts a given IPv6 socket address to a human-readable string.
     *
     * The IPv6 socket address string is formatted as "[<ipv6 address>]:<port>".
     *
     * If the resulting string does not fit in @p aBuffer (within its @p aSize characters), the string will be
     * truncated but the outputted string is always null-terminated.
     *
     * @param[out] aBuffer   A pointer to a char array to output the string (MUST NOT be NULL).
     * @param[in]  aSize     The size of @p aBuffer (in bytes).
     */
    void ToString(char *aBuffer, uint16_t aSize) const;

private:
    void ToString(StringWriter &aWriter) const;
};

/**
 * @}
 */

} // namespace Ip6

DefineCoreType(otMessageInfo, Ip6::MessageInfo);
DefineCoreType(otSockAddr, Ip6::SockAddr);

} // namespace ot

#endif // NET_SOCKET_HPP_
