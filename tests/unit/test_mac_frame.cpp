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

#include "common/code_utils.hpp"
#include "common/debug.hpp"
#include "mac/mac.hpp"
#include "mac/mac_frame.hpp"
#include "radio/radio.hpp"

#include "test_platform.h"
#include "test_util.h"

namespace ot {

bool CompareReversed(const uint8_t *aFirst, const uint8_t *aSecond, uint16_t aLength)
{
    bool matches = true;

    for (uint16_t i = 0; i < aLength; i++)
    {
        if (aFirst[i] != aSecond[aLength - 1 - i])
        {
            matches = false;
            break;
        }
    }

    return matches;
}

void TestMacAddress(void)
{
    const uint8_t           kExtAddr[OT_EXT_ADDRESS_SIZE] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0};
    const Mac::ShortAddress kShortAddr                    = 0x1234;

    ot::Instance *  instance;
    Mac::Address    addr;
    Mac::ExtAddress extAddr;
    uint8_t         buffer[OT_EXT_ADDRESS_SIZE];

    instance = testInitInstance();
    VerifyOrQuit(instance != NULL, "NULL instance\n");

    // Mac::ExtAddress

    extAddr.GenerateRandom();
    VerifyOrQuit(extAddr.IsLocal(), "Random Extended Address should have its Local bit set");
    VerifyOrQuit(!extAddr.IsGroup(), "Random Extended Address should not have its Group bit set");

    extAddr.CopyTo(buffer);
    VerifyOrQuit(memcmp(extAddr.m8, buffer, OT_EXT_ADDRESS_SIZE) == 0, "ExtAddress::CopyTo() failed");

    extAddr.CopyTo(buffer, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(CompareReversed(extAddr.m8, buffer, OT_EXT_ADDRESS_SIZE), "ExtAddress::CopyTo() failed");

    extAddr.Set(kExtAddr);
    VerifyOrQuit(memcmp(extAddr.m8, kExtAddr, OT_EXT_ADDRESS_SIZE) == 0, "ExtAddress::Set() failed");

    extAddr.Set(kExtAddr, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(CompareReversed(extAddr.m8, kExtAddr, OT_EXT_ADDRESS_SIZE), "ExtAddress::Set() failed");

    extAddr.SetLocal(true);
    VerifyOrQuit(extAddr.IsLocal(), "ExtAddress::SetLocal() failed");
    extAddr.SetLocal(false);
    VerifyOrQuit(!extAddr.IsLocal(), "ExtAddress::SetLocal() failed");
    extAddr.ToggleLocal();
    VerifyOrQuit(extAddr.IsLocal(), "ExtAddress::SetLocal() failed");
    extAddr.ToggleLocal();
    VerifyOrQuit(!extAddr.IsLocal(), "ExtAddress::SetLocal() failed");

    extAddr.SetGroup(true);
    VerifyOrQuit(extAddr.IsGroup(), "ExtAddress::SetGroup() failed");
    extAddr.SetGroup(false);
    VerifyOrQuit(!extAddr.IsGroup(), "ExtAddress::SetGroup() failed");
    extAddr.ToggleGroup();
    VerifyOrQuit(extAddr.IsGroup(), "ExtAddress::SetGroup() failed");
    extAddr.ToggleGroup();
    VerifyOrQuit(!extAddr.IsGroup(), "ExtAddress::SetGroup() failed");

    // Mac::Address

    VerifyOrQuit(addr.IsNone(), "Address constructor failed");
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeNone, "Address::GetType() failed");

    addr.SetShort(kShortAddr);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeShort, "Address::GetType() failed");
    VerifyOrQuit(addr.IsShort(), "Address::SetShort() failed");
    VerifyOrQuit(!addr.IsExtended(), "Address::SetShort() failed");
    VerifyOrQuit(addr.GetShort() == kShortAddr, "Address::GetShort() failed");

    addr.SetExtended(extAddr);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeExtended, "Address::GetType() failed");
    VerifyOrQuit(!addr.IsShort(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.IsExtended(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.GetExtended() == extAddr, "Address::GetExtended() failed");

    addr.SetExtended(extAddr.m8, Mac::ExtAddress::kReverseByteOrder);
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeExtended, "Address::GetType() failed");
    VerifyOrQuit(!addr.IsShort(), "Address::SetExtended() failed");
    VerifyOrQuit(addr.IsExtended(), "Address::SetExtended() failed");
    VerifyOrQuit(CompareReversed(addr.GetExtended().m8, extAddr.m8, OT_EXT_ADDRESS_SIZE),
                 "Address::SetExtended() reverse byte order failed");

    addr.SetNone();
    VerifyOrQuit(addr.GetType() == Mac::Address::kTypeNone, "Address::GetType() failed");
    VerifyOrQuit(addr.IsNone(), "Address:SetNone() failed");
    VerifyOrQuit(!addr.IsShort(), "Address::SetNone() failed");
    VerifyOrQuit(!addr.IsExtended(), "Address::SetNone() failed");

    VerifyOrQuit(!addr.IsBroadcast(), "Address:IsBroadcast() failed");
    VerifyOrQuit(!addr.IsShortAddrInvalid(), "Address:IsShortAddrInvalid() failed");

    addr.SetExtended(extAddr);
    VerifyOrQuit(!addr.IsBroadcast(), "Address:IsBroadcast() failed");
    VerifyOrQuit(!addr.IsShortAddrInvalid(), "Address:IsShortAddrInvalid() failed");

    addr.SetShort(kShortAddr);
    VerifyOrQuit(!addr.IsBroadcast(), "Address:IsBroadcast() failed");
    VerifyOrQuit(!addr.IsShortAddrInvalid(), "Address:IsShortAddrInvalid() failed");

    addr.SetShort(Mac::kShortAddrBroadcast);
    VerifyOrQuit(addr.IsBroadcast(), "Address:IsBroadcast() failed");
    VerifyOrQuit(!addr.IsShortAddrInvalid(), "Address:IsShortAddrInvalid() failed");

    addr.SetShort(Mac::kShortAddrInvalid);
    VerifyOrQuit(!addr.IsBroadcast(), "Address:IsBroadcast() failed");
    VerifyOrQuit(addr.IsShortAddrInvalid(), "Address:IsShortAddrInvalid() failed");

    testFreeInstance(instance);
}

void CompareNetworkName(const Mac::NetworkName &aNetworkName, const char *aNameString)
{
    uint8_t len = static_cast<uint8_t>(strlen(aNameString));

    VerifyOrQuit(strcmp(aNetworkName.GetAsCString(), aNameString) == 0, "NetworkName does not match expected value");

    VerifyOrQuit(aNetworkName.GetAsData().GetLength() == len, "NetworkName:GetAsData().GetLength() is incorrect");
    VerifyOrQuit(memcmp(aNetworkName.GetAsData().GetBuffer(), aNameString, len) == 0,
                 "NetworkName:GetAsData().GetBuffer() is incorrect");
}

void TestMacNetworkName(void)
{
    const char kEmptyName[]   = "";
    const char kName1[]       = "network";
    const char kName2[]       = "network-name";
    const char kLongName[]    = "0123456789abcdef";
    const char kTooLongName[] = "0123456789abcdef0";

    char             buffer[sizeof(kTooLongName) + 2];
    uint8_t          len;
    Mac::NetworkName networkName;

    CompareNetworkName(networkName, kEmptyName);

    SuccessOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))), "NetworkName::Set() failed");
    CompareNetworkName(networkName, kName1);

    VerifyOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))) == OT_ERROR_ALREADY,
                 "NetworkName::Set() accepted same name without returning OT_ERROR_ALREADY");
    CompareNetworkName(networkName, kName1);

    VerifyOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1) - 1)) == OT_ERROR_ALREADY,
                 "NetworkName::Set() accepted same name without returning OT_ERROR_ALREADY");

    SuccessOrQuit(networkName.Set(Mac::NameData(kName2, sizeof(kName2))), "NetworkName::Set() failed");
    CompareNetworkName(networkName, kName2);

    SuccessOrQuit(networkName.Set(Mac::NameData(kEmptyName, 0)), "NetworkName::Set() failed");
    CompareNetworkName(networkName, kEmptyName);

    SuccessOrQuit(networkName.Set(Mac::NameData(kLongName, sizeof(kLongName))), "NetworkName::Set() failed");
    CompareNetworkName(networkName, kLongName);

    VerifyOrQuit(networkName.Set(Mac::NameData(kLongName, sizeof(kLongName) - 1)) == OT_ERROR_ALREADY,
                 "NetworkName::Set() accepted same name without returning OT_ERROR_ALREADY");

    SuccessOrQuit(networkName.Set(Mac::NameData(NULL, 0)), "NetworkName::Set() failed");
    CompareNetworkName(networkName, kEmptyName);

    SuccessOrQuit(networkName.Set(Mac::NameData(kName1, sizeof(kName1))), "NetworkName::Set() failed");

    VerifyOrQuit(networkName.Set(Mac::NameData(kTooLongName, sizeof(kTooLongName))) == OT_ERROR_INVALID_ARGS,
                 "NetworkName::Set() accepted an invalid (too long) name");

    CompareNetworkName(networkName, kName1);

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, 1);
    VerifyOrQuit(len == 1, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[0] == kName1[0], "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[1] == 'a', "NameData::CopyTo() failed");

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, sizeof(kName1) - 1);
    VerifyOrQuit(len == sizeof(kName1) - 1, "NameData::CopyTo() failed");
    VerifyOrQuit(memcmp(buffer, kName1, sizeof(kName1) - 1) == 0, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[sizeof(kName1)] == 'a', "NameData::CopyTo() failed");

    memset(buffer, 'a', sizeof(buffer));
    len = networkName.GetAsData().CopyTo(buffer, sizeof(buffer));
    VerifyOrQuit(len == sizeof(kName1) - 1, "NameData::CopyTo() failed");
    VerifyOrQuit(memcmp(buffer, kName1, sizeof(kName1) - 1) == 0, "NameData::CopyTo() failed");
    VerifyOrQuit(buffer[sizeof(kName1)] == 0, "NameData::CopyTo() failed");
}

void TestMacHeader(void)
{
    static const struct
    {
        uint16_t fcf;
        uint8_t  secCtl;
        uint8_t  headerLength;
        uint8_t  footerLength;
    } tests[] = {
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrNone, 0, 3, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrShort, 0, 7, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrNone | Mac::Frame::kFcfSrcAddrExt, 0, 13, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrNone, 0, 7, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrNone, 0, 13, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort, 0, 11, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrExt, 0, 17, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrShort, 0, 17, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrExt, 0, 23, 2},

        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression,
         0, 9, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrExt |
             Mac::Frame::kFcfPanidCompression,
         0, 15, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression,
         0, 15, 2},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrExt | Mac::Frame::kFcfSrcAddrExt |
             Mac::Frame::kFcfPanidCompression,
         0, 21, 2},

        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression | Mac::Frame::kFcfSecurityEnabled,
         Mac::Frame::kSecMic32 | Mac::Frame::kKeyIdMode1, 15, 6},
        {Mac::Frame::kFcfFrameVersion2006 | Mac::Frame::kFcfDstAddrShort | Mac::Frame::kFcfSrcAddrShort |
             Mac::Frame::kFcfPanidCompression | Mac::Frame::kFcfSecurityEnabled,
         Mac::Frame::kSecMic32 | Mac::Frame::kKeyIdMode2, 19, 6},
    };

    for (unsigned i = 0; i < OT_ARRAY_LENGTH(tests); i++)
    {
        uint8_t      psdu[Mac::Frame::kMtu];
        Mac::TxFrame frame;

        frame.mPsdu   = psdu;
        frame.mLength = 0;

        frame.InitMacHeader(tests[i].fcf, tests[i].secCtl);

        VerifyOrQuit(frame.GetHeaderLength() == tests[i].headerLength, "MacHeader test failed");
        VerifyOrQuit(frame.GetFooterLength() == tests[i].footerLength, "MacHeader test failed");
        VerifyOrQuit(frame.GetLength() == tests[i].headerLength + tests[i].footerLength, "MacHeader test failed");
    }
}

void VerifyChannelMaskContent(const Mac::ChannelMask &aMask, uint8_t *aChannels, uint8_t aLength)
{
    uint8_t index = 0;
    uint8_t channel;

    for (channel = Radio::kChannelMin; channel <= Radio::kChannelMax; channel++)
    {
        if (index < aLength)
        {
            if (channel == aChannels[index])
            {
                index++;
                VerifyOrQuit(aMask.ContainsChannel(channel), "ChannelMask.ContainsChannel() failed");
            }
            else
            {
                VerifyOrQuit(!aMask.ContainsChannel(channel), "ChannelMask.ContainsChannel() failed");
            }
        }
    }

    index   = 0;
    channel = Mac::ChannelMask::kChannelIteratorFirst;

    while (aMask.GetNextChannel(channel) == OT_ERROR_NONE)
    {
        VerifyOrQuit(channel == aChannels[index++], "ChannelMask.GetNextChannel() failed");
    }

    VerifyOrQuit(index == aLength, "ChannelMask.GetNextChannel() failed");

    if (aLength == 1)
    {
        VerifyOrQuit(aMask.IsSingleChannel(), "ChannelMask.IsSingleChannel() failed");
    }
    else
    {
        VerifyOrQuit(!aMask.IsSingleChannel(), "ChannelMask.IsSingleChannel() failed");
    }

    VerifyOrQuit(aLength == aMask.GetNumberOfChannels(), "ChannelMask.GetNumberOfChannels() failed");
}

void TestMacChannelMask(void)
{
    uint8_t all_channels[] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26};
    uint8_t channels1[]    = {11, 14, 15, 16, 17, 20, 21, 22, 24, 25};
    uint8_t channels2[]    = {14, 21, 26};
    uint8_t channels3[]    = {14, 21};
    uint8_t channles4[]    = {20};

    Mac::ChannelMask mask1;
    Mac::ChannelMask mask2(Radio::kSupportedChannels);

    printf("Testing Mac::ChannelMask\n");

    VerifyOrQuit(mask1.IsEmpty(), "ChannelMask.IsEmpty failed");
    printf("empty = %s\n", mask1.ToString().AsCString());

    VerifyOrQuit(!mask2.IsEmpty(), "ChannelMask.IsEmpty failed");
    VerifyOrQuit(mask2.GetMask() == Radio::kSupportedChannels, "ChannelMask.GetMask() failed");
    printf("all_channels = %s\n", mask2.ToString().AsCString());

    mask1.SetMask(Radio::kSupportedChannels);
    VerifyOrQuit(!mask1.IsEmpty(), "ChannelMask.IsEmpty failed");
    VerifyOrQuit(mask1.GetMask() == Radio::kSupportedChannels, "ChannelMask.GetMask() failed");

    VerifyChannelMaskContent(mask1, all_channels, sizeof(all_channels));

    // Test ChannelMask::RemoveChannel()
    for (uint8_t index = 0; index < sizeof(all_channels) - 1; index++)
    {
        mask1.RemoveChannel(all_channels[index]);
        VerifyChannelMaskContent(mask1, &all_channels[index + 1], sizeof(all_channels) - 1 - index);
    }

    mask1.Clear();
    VerifyOrQuit(mask1.IsEmpty(), "ChannelMask.IsEmpty failed");
    VerifyChannelMaskContent(mask1, NULL, 0);

    for (uint16_t index = 0; index < sizeof(channels1); index++)
    {
        mask1.AddChannel(channels1[index]);
    }

    printf("channels1 = %s\n", mask1.ToString().AsCString());

    VerifyOrQuit(!mask1.IsEmpty(), "ChannelMask.IsEmpty failed");
    VerifyChannelMaskContent(mask1, channels1, sizeof(channels1));

    mask2.Clear();

    for (uint16_t index = 0; index < sizeof(channels2); index++)
    {
        mask2.AddChannel(channels2[index]);
    }

    printf("channels2 = %s\n", mask2.ToString().AsCString());

    VerifyOrQuit(!mask2.IsEmpty(), "ChannelMask.IsEmpty failed");
    VerifyChannelMaskContent(mask2, channels2, sizeof(channels2));

    mask1.Intersect(mask2);
    VerifyChannelMaskContent(mask1, channels3, sizeof(channels3));

    mask2.Clear();
    mask2.AddChannel(channles4[0]);
    VerifyChannelMaskContent(mask2, channles4, sizeof(channles4));

    printf("channels4 = %s\n", mask2.ToString().AsCString());

    mask1.Clear();
    mask2.Clear();
    VerifyOrQuit(mask1 == mask2, "ChannelMask.operator== failed");

    mask1.SetMask(Radio::kSupportedChannels);
    mask2.SetMask(Radio::kSupportedChannels);
    VerifyOrQuit(mask1 == mask2, "ChannelMask.operator== failed");

    mask1.Clear();
    VerifyOrQuit(mask1 != mask2, "ChannelMask.operator== failed");
}

void TestMacFrameApi(void)
{
    uint8_t ack_psdu1[] = {0x02, 0x10, 0x5e, 0xd2, 0x9b};

    Mac::Frame frame;

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    uint8_t data_psdu1[] = {0x29, 0xee, 0x53, 0xce, 0xfa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x05,
                            0x00, 0x00, 0x00, 0x00, 0x0a, 0x6e, 0x16, 0x0d, 0x01, 0x00, 0x00, 0x00, 0x01};
    otError error;
    uint8_t scf; // SecurityControlField
#endif

    // Imm-Ack, Sequence Number: 94
    //   Frame Control Field: 0x1002
    //     .... .... .... .010 = Frame Type: Ack (0x2)
    //     .... .... .... 0... = Security Enabled: False
    //     .... .... ...0 .... = Frame Pending: False
    //     .... .... ..0. .... = Acknowledge Request: False
    //     .... .... .0.. .... = PAN ID Compression: False
    //     .... ...0 .... .... = Sequence Number Suppression: False
    //     .... ..0. .... .... = Information Elements Present: False
    //     .... 00.. .... .... = Destination Addressing Mode: None (0x0)
    //     ..01 .... .... .... = Frame Version: IEEE Std 802.15.4-2006 (1)
    //     00.. .... .... .... = Source Addressing Mode: None (0x0)
    //   Sequence Number: 94
    //   FCS: 0x9bd2 (Correct)
    frame.mPsdu   = ack_psdu1;
    frame.mLength = sizeof(ack_psdu1);
    VerifyOrQuit(frame.GetType() == Mac::Frame::kFcfFrameAck, "Mac::Frame::GetType() failed\n");
    VerifyOrQuit(frame.GetSecurityEnabled() == false, "Mac::Frame::GetSecurityEnabled() failed\n");
    VerifyOrQuit(frame.GetFramePending() == false, "Mac::Frame::GetFramePendIng() failed\n");
    VerifyOrQuit(frame.GetAckRequest() == false, "Mac::Frame::GetAckRequest failed\n");
    VerifyOrQuit(frame.IsIePresent() == false, "Mac::Frame::IsIePresent failed\n");
    VerifyOrQuit(frame.IsDstPanIdPresent() == false, "Mac::Frame::IsDstPanIdPresent failed\n");
    VerifyOrQuit(frame.IsDstAddrPresent() == false, "Mac::Frame::IsDstAddrPresent failed\n");
    VerifyOrQuit(frame.GetVersion() == Mac::Frame::kFcfFrameVersion2006, "Mac::Frame::GetVersion failed\n");
    VerifyOrQuit(frame.IsSrcAddrPresent() == false, "Mac::Frame::IsSrcAddrPresent failed\n");
    VerifyOrQuit(frame.GetSequence() == 94, "Mac::Frame::GetSequence failed\n");

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // IEEE 802.15.4-2015 Data
    //   Sequence Number: 83
    //   Destination PAN: 0xface
    //   Destination: 16:6e:0a:00:00:00:00:01
    //   Extended Source: 16:6e:0a:00:00:00:00:05
    //   Auxiliary Security Header
    //     Security Control Field: 0x0d
    frame.mPsdu   = data_psdu1;
    frame.mLength = sizeof(data_psdu1);
    VerifyOrQuit(frame.IsVersion2015() == true, "Mac::Frame::IsVersion2015 failed\n");
    VerifyOrQuit(frame.IsDstPanIdPresent() == true, "Mac::Frame::IsDstPanIdPresent failed\n");
    VerifyOrQuit(frame.IsDstAddrPresent() == true, "Mac::Frame::IsDstAddrPresent failed\n");
    VerifyOrQuit(frame.IsSrcAddrPresent() == true, "Mac::Frame::IsSrcAddrPresent failed\n");
    VerifyOrQuit((error = frame.GetSecurityControlField(scf)) == OT_ERROR_NONE,
                 "Mac::Frame::GetSecurityControlField failed\n");
    VerifyOrQuit(scf == 0x0d, "Mac::Frame::GetSecurityControlField value failed\n");
    frame.SetSecurityControlField(0xff);
    VerifyOrQuit((error = frame.GetSecurityControlField(scf)) == OT_ERROR_NONE,
                 "Mac::Frame::GetSecurityControlField failed\n");
    VerifyOrQuit(scf == 0xff, "Mac::Frame::SetSecurityControlField value failed\n");
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
}

} // namespace ot

int main(void)
{
    ot::TestMacAddress();
    ot::TestMacNetworkName();
    ot::TestMacHeader();
    ot::TestMacChannelMask();
    ot::TestMacFrameApi();
    printf("All tests passed\n");
    return 0;
}
