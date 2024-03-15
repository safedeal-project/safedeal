// Copyright (c) 2014-2016 The Dash developers
// Copyright (c) 2016-2020 The PIVX developers
// Copyright (c) 2021-2022 The DECENOMY Core Developers
// Copyright (c) 2022-2023 The SafeDeal Core Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "messagesigner.h"
#include "net.h"
#include "netmessagemaker.h"
#include "spork.h"
#include "sporkdb.h"
#include <iostream>

#define MAKE_SPORK_DEF(name, defaultValue) CSporkDef(name, defaultValue, #name)

std::vector<CSporkDef> sporkDefs = {
    MAKE_SPORK_DEF(SPORK_8_MASTERNODE_PAYMENT_ENFORCEMENT,      4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_14_MIN_PROTOCOL_ACCEPTED,              4070908800ULL), // OFF

    MAKE_SPORK_DEF(SPORK_101_SERVICES_ENFORCEMENT,              4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_102_FORCE_ENABLED_MASTERNODE ,         4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_103_PING_MESSAGE_SALT,                             0), // ON
    MAKE_SPORK_DEF(SPORK_104_MAX_BLOCK_TIME,                    4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_105_MAX_BLOCK_SIZE,                    4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_106_STAKING_SKIP_MN_SYNC,              4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_107_IGNORE_COLLATERAL_CONFIRMATIONS,   4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_108_FORCE_MASTERNODE_MIN_AGE,          4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_109_FORCE_ENABLED_VOTED_MASTERNODE,    4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_110_FORCE_ENABLED_MASTERNODE_PAYMENT,  4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_111_ALLOW_DUPLICATE_MN_IPS,                        1), // ON
    MAKE_SPORK_DEF(SPORK_112_MASTERNODE_LAST_PAID_V2,           4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_113_RECONSIDER_WINDOW_ENFORCEMENT,                 1), // ON
    MAKE_SPORK_DEF(SPORK_114_MN_PAYMENT_V2,                     4070908800ULL), // ON
    MAKE_SPORK_DEF(SPORK_115_MN_COLLATERAL_WINDOW,                          1), // ON
    MAKE_SPORK_DEF(SPORK_116_TX_FILTERING_ENFORCEMENT,                      0), // ON


    MAKE_SPORK_DEF(SPORK_2_NOOP,                                4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_3_NOOP,                                4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_5_NOOP,                                4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_7_NOOP,                                4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_9_NOOP,                                4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_10_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_11_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_12_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_13_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_15_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_16_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_17_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_18_NOOP,                               4070908800ULL), // OFF
    MAKE_SPORK_DEF(SPORK_19_NOOP,                               4070908800ULL), // OFF
};

CSporkManager sporkManager;
std::map<uint256, CSporkMessage> mapSporks;

CSporkManager::CSporkManager()
{
    for (auto& sporkDef : sporkDefs) {
        sporkDefsById.emplace(sporkDef.sporkId, &sporkDef);
        sporkDefsByName.emplace(sporkDef.name, &sporkDef);
    }
}

void CSporkManager::Clear()
{
    strMasterPrivKey = "";
    mapSporksActive.clear();
}

// SafeDeal: on startup load spork values from previous session if they exist in the sporkDB
void CSporkManager::LoadSporksFromDB()
{
    for (const auto& sporkDef : sporkDefs) {
        // attempt to read spork from sporkDB
        CSporkMessage spork;
        if (!pSporkDB->ReadSpork(sporkDef.sporkId, spork)) {
            LogPrintf("%s : no previous value for %s found in database\n", __func__, sporkDef.name);
            continue;
        }

        // add spork to memory
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[spork.nSporkID] = spork;
        std::time_t result = spork.nValue;
        // If SPORK Value is greater than 1,000,000 assume it's actually a Date and then convert to a more readable format
        std::string sporkName = sporkManager.GetSporkNameByID(spork.nSporkID);
        if (spork.nValue > 1000000) {
            char* res = std::ctime(&result);
            LogPrintf("%s : loaded spork %s with value %d : %s\n", __func__, sporkName.c_str(), spork.nValue,
                      ((res) ? res : "no time") );
        } else {
            LogPrintf("%s : loaded spork %s with value %d\n", __func__,
                      sporkName, spork.nValue);
        }
    }
}

void CSporkManager::ProcessSpork(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (fLiteMode) return; // disable all masternode related functionality

    if (strCommand == NetMsgType::SPORK) {
        CSporkMessage spork;
        vRecv >> spork;
        
        // Ignore spork messages about unknown/deleted sporks
        std::string strSpork = sporkManager.GetSporkNameByID(spork.nSporkID);
        if (strSpork == "Unknown") return;

        // Do not accept sporks signed way too far into the future
        if (spork.nTimeSigned > GetAdjustedTime() + 2 * 60 * 60) {
            LOCK(cs_main);
            LogPrintf("%s : ERROR: too far into the future\n", __func__);
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        if (Params().GetConsensus().NetworkUpgradeActive(chainActive.Tip()->nHeight, Consensus::UPGRADE_TIME_PROTOCOL_V2) &&
            spork.nMessVersion != MessageVersion::MESS_VER_HASH) {
            LogPrintf("%s : nMessVersion=%d not accepted anymore\n", __func__, spork.nMessVersion);
            return;
        }

        uint256 hash = spork.GetHash();
        std::string sporkName = sporkManager.GetSporkNameByID(spork.nSporkID);
        {
            LOCK(cs);
            if (mapSporksActive.count(spork.nSporkID)) {
                // spork is active
                if (mapSporksActive[spork.nSporkID].nTimeSigned > spork.nTimeSigned) {
                    // spork in memory has been signed more recently
                    LogPrintf("%s : spork %d (%s) in memory is more recent: %d >= %d\n", __func__,
                            spork.nSporkID, sporkName,
                            mapSporksActive[spork.nSporkID].nTimeSigned, spork.nTimeSigned);
                    return;
                } else if(mapSporksActive[spork.nSporkID].nTimeSigned == spork.nTimeSigned) {
                    // nothing to do
                    return;
                } else {
                    // update active spork
                    LogPrintf("%s : got updated spork %d (%s) with value %d (signed at %d) \n", __func__,
                            spork.nSporkID, sporkName, spork.nValue, spork.nTimeSigned);
                }
            } else {
                // spork is not active
                LogPrintf("%s : got new spork %d (%s) with value %d (signed at %d) \n", __func__,
                        spork.nSporkID, sporkName, spork.nValue, spork.nTimeSigned);
            }
        }

        const bool fRequireNew = spork.nTimeSigned >= Params().GetConsensus().nTime_EnforceNewSporkKey;
        bool fValidSig = spork.CheckSignature();
        if (!fValidSig && !fRequireNew) {
            // See if window is open that allows for old spork key to sign messages
            if (GetAdjustedTime() < Params().GetConsensus().nTime_RejectOldSporkKey) {
                CPubKey pubkeyold = spork.GetPublicKeyOld();
                fValidSig = spork.CheckSignature(pubkeyold);
            }
        }

        if (!fValidSig) {
            LOCK(cs_main);
            LogPrintf("%s : Invalid Signature\n", __func__);
            Misbehaving(pfrom->GetId(), 100);
            return;
        }

        {
            LOCK(cs);
            mapSporks[hash] = spork;
            mapSporksActive[spork.nSporkID] = spork;
        }
        spork.Relay();

        // SafeDeal: add to spork database.
        pSporkDB->WriteSpork(spork.nSporkID, spork);
        if (spork.nSporkID == SPORK_116_TX_FILTERING_ENFORCEMENT) {
            LogPrintf("Spork::ExecuteSpork -- Initialize TX filter list\n");
            filter.BuildTxFilter();
        }
    }
    if (strCommand == NetMsgType::GETSPORKS) {
        LOCK(cs);
        std::map<SporkId, CSporkMessage>::iterator it = mapSporksActive.begin();

        while (it != mapSporksActive.end()) {
            g_connman->PushMessage(pfrom, CNetMsgMaker(pfrom->GetSendVersion()).Make(NetMsgType::SPORK, it->second));
            it++;
        }
    }
}

bool CSporkManager::UpdateSpork(SporkId nSporkID, int64_t nValue)
{
    CSporkMessage spork = CSporkMessage(nSporkID, nValue, GetTime());

    if(spork.Sign(strMasterPrivKey)){
        spork.Relay();
        LOCK(cs);
        mapSporks[spork.GetHash()] = spork;
        mapSporksActive[nSporkID] = spork;
        return true;
    }

    return false;
}

// grab the spork value, and see if it's off
bool CSporkManager::IsSporkActive(SporkId nSporkID)
{
    return GetSporkValue(nSporkID) < GetAdjustedTime();
}

// grab the value of the spork on the network, or the default
int64_t CSporkManager::GetSporkValue(SporkId nSporkID)
{
    LOCK(cs);

    if (mapSporksActive.count(nSporkID)) {
        return mapSporksActive[nSporkID].nValue;

    } else {
        auto it = sporkDefsById.find(nSporkID);
        if (it != sporkDefsById.end()) {
            return it->second->defaultValue;
        } else {
            LogPrintf("%s : Unknown Spork %d\n", __func__, nSporkID);
        }
    }

    return -1;
}

SporkId CSporkManager::GetSporkIDByName(std::string strName)
{
    auto it = sporkDefsByName.find(strName);
    if (it == sporkDefsByName.end()) {
        LogPrintf("%s : Unknown Spork name '%s'\n", __func__, strName);
        return SPORK_INVALID;
    }
    return it->second->sporkId;
}

std::string CSporkManager::GetSporkNameByID(SporkId nSporkID)
{
    auto it = sporkDefsById.find(nSporkID);
    if (it == sporkDefsById.end()) {
        LogPrintf("%s : Unknown Spork ID %d\n", __func__, nSporkID);
        return "Unknown";
    }
    return it->second->name;
}

bool CSporkManager::SetPrivKey(std::string strPrivKey)
{
    CSporkMessage spork;

    spork.Sign(strPrivKey);

    const bool fRequireNew = GetTime() >= Params().GetConsensus().nTime_EnforceNewSporkKey;
    bool fValidSig = spork.CheckSignature();
    if (!fValidSig && !fRequireNew) {
        // See if window is open that allows for old spork key to sign messages
        if (GetAdjustedTime() < Params().GetConsensus().nTime_RejectOldSporkKey) {
            CPubKey pubkeyold = spork.GetPublicKeyOld();
            fValidSig = spork.CheckSignature(pubkeyold);
        }
    }
    if (fValidSig) {
        LOCK(cs);
        // Test signing successful, proceed
        LogPrintf("%s : Successfully initialized as spork signer\n", __func__);
        strMasterPrivKey = strPrivKey;
        return true;
    }

    return false;
}

std::string CSporkManager::ToString() const
{
    LOCK(cs);
    return strprintf("Sporks: %llu", mapSporksActive.size());
}

uint256 CSporkMessage::GetSignatureHash() const
{
    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << nMessVersion;
    ss << nSporkID;
    ss << nValue;
    ss << nTimeSigned;
    return ss.GetHash();
}

std::string CSporkMessage::GetStrMessage() const
{
    return std::to_string(nSporkID) +
            std::to_string(nValue) +
            std::to_string(nTimeSigned);
}

const CPubKey CSporkMessage::GetPublicKey(std::string& strErrorRet) const
{
    return CPubKey(ParseHex(Params().GetConsensus().strSporkPubKey));
}

const CPubKey CSporkMessage::GetPublicKeyOld() const
{
    return CPubKey(ParseHex(Params().GetConsensus().strSporkPubKeyOld));
}

void CSporkMessage::Relay()
{
    CInv inv(MSG_SPORK, GetHash());
    g_connman->RelayInv(inv);
}

CTxFilterManager::CTxFilterManager()
{
    bool txFilterState = false;
    int txFilterTarget = 0;
    InitTxFilter();
}

void CTxFilterManager::InitTxFilter()
{
    std::vector<std::string> pba {
    "SYmgtbrmouVwnHqtjxTjqpmCKVKDz7eTp2",
    "SfsfbNsC2B6gA1hZ2KPedteSh3MucLm4yX",
    "SeWmpsyXh9mcXe1zxZLyWbABqbSiTjnqts",
    "SfKwYZ6Nz5iHrZpcoFqfx39i7LDkJeHJUj",
    "ShbSPKotKsv9PBUSNSM2iwGZaioaJxTS8p",
    "SPu54N65LdHAXwvuQpBtw38CznRw9sDhc8",
    "SRMkrKXQ1FgRHwoBoW7cDSh6C5nJuwFr7R",
    "Sd8AY8AM5ArmbQmEMf3N9C57V8f1kkAtyg",
    "SUejJcFHgpeimYarM3y5KgUe5arPJDEE5n",
    "SdK8so5UUY7NFyjExwD1Z5sjupDq5JsCA5",
    "SfWHCDxWb6B28NaAEAiG2qXsTXFEUxjher",
    "SbqKSqV9jz3KsMSk3knyVRSpqqCwUwNmmU",
    "ShzTqT2nAg2X1PkYwLcz1GnHJkg7NX55Fh",
    "SiK3vGAQszYTbXvKNoo6M3YMiMtjESR6cu",
    "SbihfB6gKtMgzBVwJdbwfPsc5eN6k1qnjE",
    "SjL4SfGXxtwvfTseKZPKhM1nyfvrEmT8gC",
    "Sayok7DbWGUzF4qs2jdxj3TVSH5WtzDfp5",
    "Sap5cidKzpyw49iiPKRb5QHEHdxJWKuXLj",
    "SR4LocL8MMwrjnHDLggq3RQ1i8gStjnbWv",
    "SWwdb6WtPnv96BhNXXoWo3HTyMuw5NseVa",
    "SUrj28tAck7Jtpec1g4jwVvpvh6tZAGfhE",
    "SRwhk3HyezHd5zgQSY7jK4wuiB5dyKwB5S",
    "SVYEXBv9P9SzBMXjWq1ShFL6FGsNTMQquY",
    "SbGiX7UTTS9QxzFDXz1byKZ7pzayCWTiRY",
    "SRCRgtFr7vSiD8yGHm7d6PC4So6RqN1Hwe",
    "SV5cMesfxqUQumnWhHfatcQLJtqEWpZFi6",
    "SZjn97dmbg7n2TfvuoNWrBrgErzFNA8RVx",
    "ScNBp5nuiFHHRUdeHJzqz3DVxASTLtjgP8",
    "SR2w4Dx33VHqemZQgY4eGu7dhMt6GkNxBH",
    "Sdc8qaSXtFKmNvNv4uG1D5UW9ZbnNmZVRZ",
    "ScjgMpfcnSSgMxJLtMqbKZrCHmnSdLjtdJ",
    "SZLZXaK728hJXyqVb3J8bkzC7EG8AUgz3s",
    "SfWwiNSDDEwVkxfzCE77LsVVjaLATCjUto",
    "SYK6v4w6nhkFPxM9ek5UGdzXgrxRWcaPQt",
    "SZmKoJEs1xKu7sx3PHj6dFsaUBx7Et1HFo",
    "SSmYfBHUqdHQX3wwf3pobTniWJQhUWzRLD",
    "SNsysjgVuUYa8dDqrio4XdcLT38t2EgQjZ",
    "SYmbYjooC6Mgs7TzJiyYqMBGYjGw56jghf",
    "ShvxC7WQ9mL2pmJFLUWWo8ynioADFjp9CV",
    "Sibm18U3CgNXeo9CHpvNja8ZLGkL4RhSHm",
    "SYo21rW2PC9VtSNV8zDToMkkjS43zjTEie",
    "SbLBbtYr2QFT5o6dZ6SYhi1FnGV4f4cTr8",
    "SWjrT8CyUMnqjkqd8ehtCqSk6ZKmjDuGVZ",
    "Sd6KauWXUYxsJxgtzk2iMcdopy8584TTdU",
    "SVmrj3gMA54qgM8MEnxfrqSXgqkEPGRtkG",
    "SdL1R2aLNT9hyXBFDDTP6LtTXFXUCYJs5a",
    "SNuMRJYUSzE2KY1qPyN6Kp8AMMxz4uV1q6",
    "SbbfzeKsv542yfWZ62X7VvsgAXnDPApZPQ",
    "SYp3Boyc7BSZ89k2TAWm7PwvVEbo24ekTV",
    "SStCurFi6mGHj1mKTjkM8abTop9hZL76SS",
    "SSrmqqJQhnjh8Lgbabsjrdw8Y8ok6Z4v4v",
    "SUUksTQWzmUssYsQdAHmxqPhReMnwjbddT",
    "SjG92C26w99a8RcQQfLMWgvcVvhBNRwDjo",
    "SX5r4jXuosLhRRSwT3R3wZ7FwGcFyb6t5u",
    "SRzfH4efWjM85kHJ3ubbjGitQ3gAxfZ454",
    "SZhu8wefPXbStydgdNdR7sQAtFAuVBYVyb",
    "SR6HcGhp2XceGP9hYMrV6gGWgSG6shXkr1",
    "SekwgyAgsoBQK8C9ewde2U7zHD5pej5BwF",
    "SNiLTQPtN3C9f4uvQaSzzrbb4WVRhbXfE5",
    "SbCbbBeZbBzKYQKc6dLUoX1mrvveKSFHJJ",
    "SNgAKY7JTwj6rVFH39c3irkpCSKy8CmK1q",
    "STiqwmsfqBuF5MEok62ijtV6LLcLs2QuXG",
    "Sb6b2oMVWE4wMokaAjfdbzvnrLCjMyfZEF",
    "SdXaBXAMcoJk5U4dRs5ADpwKo7cHtdqtSg",
    "SjtMGxPxBCiyTNMyY8BBEhhj6sbkLjMWdZ",
    "SVhPWmqB3T9YpetLk6pRD6reuRZYdhWZyr",
    "SQof2ZDZsy83ZEDbqtdr5XqEVxo7gpyTFU",
    "SZt1c2MFB2Yv9reCdfsdZt8tXZ3BC754DJ",
    "SdbDUeEUpHcH1KHN2FUfMNPwarSHJkX8qM",
    "SX3bk5hhLip5z8qy1CDCzhHBLnC2dPVEC8",
    "SeXbTCu1ZTbgkvMhXMiyr34NeN42ZLqcRE",
    "SRBivFv85oDxAMozujR5pn2f1vG394jqqA",
    "SSKkvY331K3kodVKDjVveCSHYyJGnb525y",
    "SYefrTMY73jroGq4sC2tQc3LXRL1KoLufM",
    "Sd7Z8Jx2Svho2xjRkXSj4dJ8fpXqDfNAMG",
    "SiWKtZksdtN9RyfPU4zun3QzeWNam6LsLm",
    "STxx5cWMNKyg79RZMuwLXXDPWiMemRewGv",
    "SNPtxr3NoLnSm9uzahYoLssLEm4CUJkLti",
    "SQFsX2wqGrxbytoB2mKGPW19J7Zu5ogqeh",
    "SQ8sETxvLitsGuV6teTVkGLEks6mSm9V5h",
    "SNpyAThp38LCHS2ZkPozYyLAoyUHDoTVMe",
    "SStp64XE1dcAZdk484xoEfTTm1zKquxmrX",
    "Sj2JwKbPXdCJ9FczQgc2pYZRb3ftZgrDR1",
    "STMTE1fsBfdtMGh5RHDfvbQy3A9ejQSuBF",
    "SVz8PJUvwMDXsxhwAnqjyZhKXZsWVx2Z8j",
    "SVnBruqc9JQeqEZExPyr5zQRFYprFoRWoM",
    "SZ1SPeDLGdFF9vtn5s2mWnLLrL17gQrfSx",
    "SjeEFNf4aWpeK7QwRoZkupdJrpYe7Xd5pW",
    "SagpojwsVo2cXorttEBiu1yG1LtLbQ5DEp",
    "ST4JLd3T3jMt4qC3kGQuAM9MZF8RSTvSMK",
    "Sia6xKzoxFjcgSkNUNkUwu7tVWixJVwpfS",
    "Scqr8Xw5GRWf6PkMVrTrGy9F6ruX7B7SVW",
    "SfopRescWkWExmTNQxs9sM8F7qpM22A7u2",
    "SbJVPNTtXiBhxWrqfrqHUrySDheptoY6gE",
    "SgYW5esZieFqdK3w2qMeGVM7K6SQ9bSS3C",
    "SUcc8xWiNBKsddnKEyrqNY9hucQbGJXmM8",
    "SPnbkmxSR8XhgfwqdE5JWnxQ6XDwZBrrQi",
    "SW8RmHBqgLN43poP6WCXTHJrwrcbJoimVw",
    "SW7pJJiZW2jMcGQg9bySHZW6mKYuKKAZds",
    "SVbGSqZgGUFuUrXPXwWARqVYxQrUSzBexZ",
    "SU5zWetuqESFkjHH9dvbcYF46xM6kUtezT",
    "SPgB57X9A1QE1e5hDPRu2xj4CpexcWfqLk",
    "SNnqBtHCh3g7ydEiYhaKfMMa6HuiVTmVnh",
    "STRdpGFpoNaTjyk5Rs1zJ8oqcvousiC4dt",
    "Se1gqvopeec1mTvvsAqZHXXVYP9tHN5dLu",
    "SZ7Ven36Ra8j8NQNRejYGYRrbMhD2vVWoK",
    "Sj7b5S88e14C69dxauh2dKNE3c9ayvXU8R",
    "SUfbTnbRXQ9XLLP4ncUFf9agtXGqawi9MD",
    "SUsnnQ2Ts6PMMVoAEo6cM7LVzzahicDRgv",
    "SamxPV2f3LFMBS5WP787Mm7AMj3yGfH2KH",
    "SbLou1aPfkPTuwSNJNXtES7zkugRHno1Ed",
    "SVgn5SXJrZnwh112ejfYjUmftAjrG98Xox",
    "Sh5oWUXRMWvoXRei7nfLsAdf26euKJ5U37",
    "Sh8NXvtD6FvrpqRW1ZqqjGX2D1GU11wauC",
    "SRkBH1zx574T6mFwXa3DMvpEuY2DwbduEh",
    "SdTVpEg899dv9NVTEwdERhKUjT5fW4NQQP",
    "ScDezao5jM1T5d9Fm9HytioTw9o8K8jZxv",
    "SThy87QWGnyrZnFXv3fCu56RqVF2feb2Ti",
    "SVPFu6qbZSC35aD8BSGex5Bk2hmqRRFwp2",
    "Sg13osNK9QSj4Gi2DqAMmks6DNRFdMUwAM",
    "Sd25fracv2Vt1YPzZgXR2WGVcHUXAPv5Wk",
    "SYaiB74fgF8RQc4HaEYu3FHpvbiLf7r7Uk",
    "SeLXNKZBXeDHWzbsmuGzQHjM64bhTuCJks",
    "SPfHeXeYHPn49FkCkXNHRrdaSFMTSsn77L",
    "SPQzRV8mF7Tdo3ziPRQhhrwS77ggP7bHKM",
    "SQQmSSNToTtTgWUH4ckzNpSa6kbmmxooty",
    "SWVJvXjTz18eMEDEPSn7tCSa4TGMJsbJGd",
    "Scd5pH5PPBcwxmGWeacXH6kvuzBkyQqCSK",
    "SSZbjrdTLZ7soEMj6g8vGVTrcfs5KMwwor",
    "SQwYA578FwgxbM5mcEKA6XrYrpgwz5tZeb",
    "SZuN4wAm3a5TxgzvyMDymMbEofemxriFty",
    "SUXG3uvGvX3xYF41wuAeEr2XetQUfUvkzJ",
    "SZb4XMbTuSA7DfeSpGM43fwNVFWEuNyRnU",
    "Sdso53D2ZWPGGTwHTAkwzuBg9HpvJuqzPq",
    "SYH3omzmHhVvhvbjC7rvF3mGxPeeRCP5He",
    "SfXf7ZgZoySiEZYmKcUBukk3eLaac8L5Li",
    "SjKzzTZcNDLZZ4UswY98Buu5WvvWd4K8U2",
    "SazNivwWJDZp3RXW24nc6Tzkj43AaVhLHh",
    "SQoZnYmgKjkcmSdFxmAEfmPhcRjKDgdhmH",
    "SQg3BhCVNjSC5rzPca33yy4fJsMFUALoeu",
    "SZBRwrdcitF1ebLzTUxwSvpwUjxLDTSG5Q",
    "SiX82QVYtq7RPhpPt2SvPRuuMnW9t5XKF7",
    "SUUaNrLuqScsJrokRbH3ymHAv29A7fGE5w",
    "SjhsMdxJuG4ZzhXBttXtrMA9FG1KdMg2n9",
    "STm42JTKKZ6dToZ7hRVXXtLmMowVSSvY1w",
    "SXq21Cpj4kYBoSK6vwu4H4qvDkmTZv3Rid",
    "SgZLtiVR6VdsWqk2AYHk6dr2nEogLb2hW8",
    "SYWtpyS21qopLLe5zhPS68unSUCqmsqhzg",
    "SZFPwzKmigy4NtM6riAXxNKd1jU7bgkjv7",
    "STTp7H9JaBWKnTH6LqJHhwQL4ar4R3c3BS",
    "SiDZvGSMc3Dzinzv1acG4GXVogRqcYEJJR",
    "SeM7SUsztSb2qFTB6Pv8hKXpLLo66dBgN3",
    "SPTRPty8Z4MM4kqUvr3Y4Vn7N1HzUY3Ryi",
    "Sf7AsrZDkJ4mW6DwksQ4oGLFtSbYdVc78r",
    "SNtmtcQhvANXauNSVPMRz54wX2dc44c2rT",
    "ShD7xiKnFSyrtGVPdgtzKM5iQpR8QDRXZb",
    "Shsc2EU9EYgKQGrHMvjgU1XYz7KBcMP9Mm",
    "SY4QzKPdqzZYenuYcyZZBkYB1rCmjaWQmc",
    "SQ2bcmpqFYMb75asSVcTwctytVyR8yRg9q",
    "SkdavmDDPkNGxArouq1LQtJGbijJauMd16",
    "SXUmVnEftkvctA8KJPDkqivKYyJrrWMWQH",
    "SiAZEe7oGFiw31cgyDmhyjtH8fk1gEHfRs",
    "SaXKtoF3v4NFd2yp4Z8fH3vfAwMh9pa2F4",
    "SX1ETunQEnhYzdCtDHquMfC951mpWeayS5",
    "Sd4sd9aGA6MNptcJ37YuJQzy3ULvDoZ2gp",
    "SjCjrj3uziEg3udEo3H9itV1CZUSjRb7yU",
    "SiKKn2uRmNK68DRAVmc82mvgMMhoH5JGWA",
    "ShpAApe9TDRRHc4XxC7ZfcTAfbBtNzjzS1",
    "SR2DmwjNRvTkpDEibNDyzNVBsvLmGrmxEe",
    "SY5P6YjyBmVVtHsEzayP3JMoq2f3Cjh6nf",
    "STHri33eu4qUdD3JRgk55SUEAMzoewdKY2",
    "SScj8rZg8FrQhUojeCNMCLQHDnSbs1JdUv",
    "SgbS9yknc5DVAqqFWZAN3KVAjSwYQiypYq",
    "SYG9H6THdQDZL7J83pFEjkpHNM73MazRK1",
    "SaJwx8RPbd77BCjDc3p9YFPyvbteuwFeUd",
    "Sb1wpHVTmk9hqDLzAZegXwFNsfhA6zHDpK",
    "SMSCNu3m15iSrZmceNeCu6Fd31xcX38jK4",
    "SRRncy4duMVPASpocscAo4WMQWBzhEpBFc",
    "SanaL4xHPLNU5QEmS85Wjw3FV2Y2h6rAd1",
    "SgLKzxQEfyk8QkFrs23sLg6CkZafGqubvx",
    "Si2jA4HYrKExyubqexEwJsmRiZFXSqpKP4",
    "STsMu96Q11uoBJPpSBZT4wgrkQZgcE3jSk",
    "Sj3nhDGhgoJ4AgQnb3ncpPzSotA5gBcVsV",
    "SUeoGC6nixZWfWGFJfNqsbPSDqGtCZ31aH",
    "SZan5fXkWbCYscasorAfD4utwtvzdWhE83",
    "STk3hsoboo8Kxm41rGchv8StVBUe4B2oEi",
    "SMUvjj5dPbFwqrvZBuYbuQ4r9gyDsLuRiT",
    "SeueRdgH33rTuHe42e5V48AmiYpc29CPFj",
    "SZKgVGkRjarcoeWny9Ms4pAsrjmsxe3Y9U",
    "SQKmNoVddSRECZMYnQ5nqoj3DQGtwWKiWX",
    "SfzuxgQXdFL33WiVps19dHt8E5HudYDiWb",
    "SfHA1JvAL93aA46sMHfcJxTfSYnu1sgBak",
    "SP2gZ44xoRDsqZEqNJuCwN66Mjev9rEY19",
    "SbZqAPb5GnddjY8ffdcYzt6LX6vbpjivLH",
    "See4h8L7MzN6u4aP2NuCMnixmxeXWohXen",
    "SZbJ7ufzALJi787D9UCvXwU5pcaypxazyi",
    "SjtU1A8K1VmzSBp9mjLNrLi8EhxGZWRgcL",
    "SetYy2vqnAYcaPy367jT3jfDxeqh4UfMnR",
    "SgGtWAmY5uYFQSLEPj4CJr18BvCGUqenfU",
    "SXts3wwYgqUcEh6X4UKXtGFQE3L3hVD4Do",
    "SjMbus7CdGb1JyRSWqJnrAXPD6oZgm4833",
    "SgJBeFS2XhznSN8LM8mb1jrGkckCggVqcS",
    "SewxEe2N43bh2aaRnPvYxbW2utRXTszN6f",
    "SbLXPBPRxK7XPfcRVhpZGHwwco9Yam2GtH",
    "SUBk58mmQRREVLvL6b2fcBP9m5NTmKvatB",
    "SbBBgER4MkYASP1vJ9mh4QqsMdjS1rSsvK",
    "SSbFr4UtDgPSfmYjipAHiQp7EnCt4TdKz8",
    "SS2BS28WikRBzdYPjpyMRkps66BrAZJ9sj",
    "SibvixmtXStUPSoHybZ6B3f1CxPNLabris",
    "SbJE6WDhgz6wS7zbtrmn2vGeoxGGCknZjG",
    "SjAfqtXckZN9vqr6KhuvjUyo1FxfSBEr4R",
    "SbL7jGhQAf6xpMmzvuycvZ6ucH7chZpr9c",
    "SaXhGQhdpTw7zRrP8FdgZEAoybBjqiK6Am",
    "SSA1sKWLHszpjGwg1Pxu2jB7MWwT1A3m2i",
    "SgvUcnx4RqhydTXAEBRyH4mjRfvo6oBF49",
    "SdA3q9HQVjMkqcb5Pw1DEBEhmvScZbzpq7",
    "ST7Zk3CrqP1Knnsp4aDFAWHFU3rgmKAH3r",
    "SheW6jsMNm3CcXAAqKtqbrLYwpuMJLQGZT",
    "SNdQzr8heAzrZZ1G3VHJUzfKzvRwP92BAt",
    "SZjj6ayhvkQPg4BKmBFB7FiwghQFV2J2QL",
    "Sk6A2swzWaRoNoTfnJerCBYUtRdy37N7h5",
    "SggUghzzFWEzXm385qHvPaKAVsiudexnGU",
    "SS38NJ6bAVZox9YG4z7HYXkFiCtARWSWcP",
    "SYwEmgdTQbov6wWsgb4mjBAZ5hA3VNGKZ5",
    "SSwpJCP8BzSoVCSFxz4bq4CBBfegnu35NK",
    "SUjNiJyrCmNSay9vRprE3v77BpcKwyzcBq",
    "SapWWRGD8dPfGVKo4Kr9zTwuZP9gLoHHWV",
    "SfYqp5xSdt4qYNXaV34YBpG17AqryEnxzQ",
    "SXrUQtBdTZrQeDsvPtmMaeEP25Yn9hG22a",
    "SgvasLLGAmrrJhrfAVSTwNDiHRU4Fa9Nka",
    "SfQhDHJZr3ScCuyJEtydZZosnrjHp7U7Fj",
    "SMfTsJTgo3KyACR6rSAnWEm9DztbowavZH",
    "SSnGsgu3voQUf5r7W4X9DCev8uhoU1KS5c",
    "ShETyC3U1j8cELyVAcBcGWJcdz4BEmC4og",
    "Sc1H22FkkzJ9TtbsZgwbCBa5mDHHkFdZMu",
    "SV5oPVpeZbP27kZmdNUdqoVCgYMb2mnr7g",
    "SVPZdaM4PmxLR8g85rsotnXRErxD8bhkEc",
    "SXV11KNLfpQimT4HJHt8oh3TS4DyP8iHt5",
    "SVU4HRqfuem3qD38cLgq85FTAChWTYNvdp",
    "SZTivXXMXgG2Yv6pHjJB2VzBeNFA7sLgfF",
    "SfgDNSPNbV8BLM3H6RSRGUzgMarcCAaYL6",
    "SUFrwypMRPC5N65Xs3RWDZTHBVCrWmPfdJ",
    "SRFGj4HoGbWxS9uXqtTTr4cB6K3nJD9M5b",
    "SXEo7tp4vVZj8zLj9ouBZtntG9m8GQJGbn",
    "SWfjLYZVfGsAxibgwHhzLtkgQCDXJyAgRv",
    "SWm1r1qdFjwreyJ48T9oea7C1pmUM28zCT",
    "SWy9Hom2jrjyNHKt7EQ6skjbidRfMvngsy",
    "Sghz24sZjB2uHpviCAxgkvGM6myhQh4LsB",
    "SgQcqK6EJEAnnLsdLPttjwgCJ72QztjGce",
    "Se58kxeZdZzShxH7FjyNEWhY1jNqWk4qvY",
    "SYBmAM6brWQotkEh18uNf4c1ks3Uhy7t4t",
    "SeBWS7mDzBPPMtwU36VexPRVfUay1pLXPr",
    "Sjup2Y1ibX9kJYWPJUQB4CbfDE6DotTun6",
    "SducyJTDzqzyUdSHdeb3NsLNKjUjk1vi9M",
    "SWibSEY2GjWqmQEZcYcpKgKXCLksxgrpP9",
    "SWzHwVhpjhPZYtsi7HCq5ECEh1316tTrZN",
    "SQWfJjPisgqn7Sq93x5StEERB6DaKFuG29",
    "SjmGK4xh2Y7oxsgEoqJG9ijdvixRQqqFQk",
    "SPuFUtQwmgPiMexRKxY4BddXJn37d8D888",
    "SRkQE4x9mN6xKwzMpEaSMY7iXByYDzLDPk",
    "Sfp3S6xFsZ7VqU3CC8NR2U18wPvfyE7GeU",
    "SQUBog8c3KJBQVtEBStiqXQQ3TwrfUx9NG",
    "SSrxHCTQhU5mRfCa9g7j5dgLEnQNnyfb4F",
    "SZaJsbznp9QhXGiarhopYMpNS14JYveyDk",
    "SRhP5GCCX7bLvtFDMymySPyrMfjy7y2WjS",
    "SYVSo31rZw9wZ1TsAi3gwZG6QPeL8FSWLR",
    "ScgAnimX57p8pNdqUHhsqc7bAkLdoHZnVx",
    "SeoKJnB8vTGGSWrgH139Sgj4wQqwdMjdzH",
    "SdE64UrfEeGiSuWt9gdr4ihsUvTfCLYxu5",
    "SWkV7ecQFSYG9KJWWdRut4ZTusoWBSArV3",
    "SdgyFruAEmvNQ5679EtfGA96xd21beqLTi",
    "SMS2Ecjtfb29H735XmQ7hbEpnHHpTquxrL",
    "SQioVd4KdpMNobC9ebvnEJv4vYEKzErtpE",
    "Sapj5mK5FqrfHEchFHETL75NpkuKusxspm",
    "SitRb6TTRqLn4C69ekR1VDzpJCtxRoUTRQ",
    "SSZ5neaqRz8HqRvLabJE1H2kAsTb7WviA4",
    "SgzuV4NiDhVtcNXzpk2VJw36vjXNVjBcJp",
    "SgX2eC9Lu4pHyfuSgJooSfxK6KKNrdXBrG",
    "Se2hpKztMmz2apezN42wEaEzRZ3CVvZwk4",
    "SdeYHKfykq2gBe9HiEb2o6AoWVv2zr3kLc",
    "Sb9AW2vn4jYaJJkB8VYcs6gtyZ5oWV2gsD",
    "SjMReU7SoKvhQE4nGvLtjPrn71JwVT8tBo",
    "Siz6WAikYYJi4wnRYxZ9y7v836K2XgqiiF",
    "SeshAwE3nXryVYTMPziNVbnsFuMsMDoGJ1",
    "Siu5KLBLfmSag4mEVkdmtxCtjELaAag3su",
    "SgaR2sUj3ZjAtANEpYLis7LLWiTsd5W6sN",
    "SgkHiDrYSkAAoBCgQiar7hgf7EuVgmNVkd",
    "SeD8m3g89LK7Z99zgpaFUv4oDzUVzY8LxK",
    "SRK7abCVy1JhqaX12FfXQu3TNGQKVAth7V",
    "Sgf8mTDYbqAuco7nVHfceKnAEsFa4toDFz",
    "SjyYbXB9RLrtf8THUmhbHutPJUXYjw94jp",
    "SecZqEgGpvBhZSNXze8tHm2vYVMG1Ayq5F",
    "SgfZHMpmGH8fTkJ6PQMUsRrMK3jHp4gbgD",
    "SjF3gZTsfNPnCcsyQJXngdm1o1ragPjvWp",
    "SduEt2xwwHA33dLmkA1h23RVpZPPCvcZTT",
    "SNzvKFXz3QtXymjuHBs7JnDFVFsnvQ2e6n",
    "SfBp4ydFGoy5YkwGdUJ83G9JbJpYNpsXDk",
    "SbpFwPzfWfherCZGeSeN5EERhBCjsK3skD",
    "ShhEj1eLvZxetUZt4hShZBFARpkMxn5FGX",
    "SVtyCvbdXys9UCv62pyHHbqekoYautuVuG",
    "SPLx1KPnx5a1vH3HkUtEYtAZVHvGTLAE6Z",
    "SZGcdZ7RkVG2cexzuK2P6dqky2JGYc2YmC",
    "SaAZW6GhjfQWck4LiSkVznm9Jx1iViPGCj",
    "SbPPPgfgp7zT7SkLUaZQS1ouhdasn375g6",
    "SUALdViJMUrs8dGfyPG9WrJwpXWn1ERMSd",
    "Sj8vUxTMumHiqYCWEEqfaaYEyeXTmPRMPw",
    "SV2MfnghAe3bsBTv6o4fACMvGj65EXuENz",
    "Sif4qgo3ZySwFuSuqw3bT7dt2piEeMjzK5",
    "SjEmwRUEJZ8oXz25PxMPW1uKWqYCGREs7v",
    "SMVHxfV2K9M5rcFYTshEtFDHg8QZNxxvAz",
    "SYM3YkcuGSz32HisA3TFUvqp2rRfzMwStg",
    "SQqmqZY7c9ekSNYKyrS2UBjGk5GpnN7zWc",
    "Sf7Didamcxf7rmvX5j9rU2aE5BZcVactnh",
    "SbbNcPJBofmwdg4qGGiycxMz7Qk6iqAsmu",
    "SjgaZTbTvdkKG3YYDZ7QdsePRcRimRopWn",
    "SjhvK6vcuNPGx4m87Kecr6DQjh8jqrPk1Y",
    "Sk6i4vjTbM1KWr5k4B4m3F7XF6w19iobYP",
    "SU4t2xAy3A3vMCrW1JMwfvANBhLu3a9zEd",
    "SZnbmK1PsGEvBstXCV68pMSQSRpraPJReA",
    "SYe567BzFGPr7ysLsL4xKUTpHyE4KwHaM6",
    "SNSZkEaGGP1eZ6cKCjGY2we6f2RdxuqtjU",
    "Sehgpn42sJ8EQ82PtxXaXkeHSsrcxAPbvA",
    "SMM287wrQZey39xZnezsxPHLj1rDbSxdGF",
    "SW9xVdZ47iUPMik2544Tr7PNvwF5vD7mTb",
    "SaLVJGWRyUB272BKowFB2VqHY7cStQUd5G",
    "SZgXvqDqpWC4pzJhybrw8NKvxHQh4JLGm9",
    "SjqkxLdNjUHYC8UXE3gRDPPLSQ8urHCDJs",
    "ScYLRAosARuu5xZC7oeCkB5rBVFDuGE6CD",
    "SfPaEqMuvYfrXzygs4QtQqMvoiuaydnNgE",
    "SRwXQvKdHMmkujUU78hi67JBdbxcBnx7NG",
    "SPckABsNCQn5xFRb7xxSUdMciqFhTe4GTV",
    "SgYvCu7HxZXd1PEcPsLKXrqzKm5e4F8ikc",
    "SULvarBRvznAPANzJCT7Mpk7DyvFBbNngV",
    "SPwvRAfd8Pno6LbWe9sBbYDC3qLQPnZKRJ",
    "SQHy3o7rsFGehWfYPw35LNo8NUSDhQ25sp",
    "STsgY4TwTZxPF1t241qxpKK1tU9MV1JZKn",
    "SbmKMG5MPkfJWVFyBKTJDtwMsRG9QV5WTf",
    "SN2xNXFh6WTZmgbRHJsdWTVCKmW2rf9TR3",
    "SSN8uBtM6jUgc6Qyfk37K1aYGF8Rw2cvKq",
    "SkSCbmhpsT7QAPX6w4QkgjU9HNmWFDS7sZ",
    "SYDujyvpFqzKP1ay1joiPF5myGBmUKR8GQ",
    "SYRb5iC7VkUh3piDEeUtE17eXuvZXn1n2v",
    "SexZYhWLbdxdBfP462HbKZ8swLbKG1i84p",
    "SQkMzV4FLGJahwNWENcUQeZVjskiudb1uG",
    "SPVievL96hhzqi8w8SBnSZGR5LCddCR5kr",
    "STb7b8SiKdNtH8JaZcgN6iNicS2Wv6J5XR",
    "SfKkYB8HvQdFPcyUsbemfQUMKekWQ17s4j",
    "SkMvqjAHNvMzk6Xn9oJB26bSkzFvzWn74Y",
    "SZRDSXtSCD2ReSFmMC4JF5et8eb8YCSWqj",
    "SgWBz7MjY5Jpy6NNwQ1rMDqDrdU4zZW9AW",
    "Sd1KrKw1tsEx8S8z2eveAC6rHR8cNqooXk",
    "SZcBwVUVn5xJRKGpNu8gaWdCPKXStvFX6f",
    "SMtCSW2HicAYD7kn6YUdcsAc9s3Y6EGW8z",
    "SRSMGPa4S4RcDJ3U8ydrpJ3LSyywVL2ebP",
    "SWWTqinsrFCBFDwnXJxHJekEmc6cpM15pt",
    "SWZmhQeX9zA9BRSAAMxtUMbboGqiwj1qUP",
    "SZYuzNAvft5zjHSGAHqdNTR6gJG6P64J2o",
    "SUw3JCtUHew4qmzXnc6zVXfmYxtDDmysv8",
    "Sgc6pTsuVkpA6v4pB4CJRBBbyUSSGfNLer",
    "SV3U12dig7KVavqv3j4DaX8qDRLwz7RJQC",
    "SWgPm83QhJZLA8X4rhsGq6xK2cTpwnAF2v",
    "SQ1S9hCpYE8d8Q9HnNEYBiafhUstA4ZMEr",
    "Sckp6A4KmoEYxUqqtYArkxsVYMuhgwg9hu",
    "SYUa1TqN1Mrr7Q7QG9DgGPZE1vLp8Svz5S",
    "Scnda5YdDUbbM9DV4Z2nsZ8LnhUyE3gqSJ",
    "SYPyFhxqpbeWy7K1h4moFLwX7XMdq6bjPx",
    "SYDdLdQDKq1TbtV3AyqekZj3qyFf1Mc1Ce",
    "SU8GpMc8TkywxSWseM787njPBsDPwmCtg9",
    "SRwM5JhmcMUmLLkRS67zpV16PbGpXaLaV6",
    "SdLyTpeyhWAshcnBbhxTY1EVhjj88AjXAw",
    "SjoR8fP7Gy6QaEh4fbb8dR4tngzuvDnmVq",
    "SYZbc2jGrkRiXPAxhwxgeAGnjsLCzqxUV5",
    "SURiHeQCuY2LTKE5sHZtMnTDdPGRE5xE2A",
    "SUfKG6cCuq1cEhFMLDt7dQ5hJEN7KMgv4G",
    "SPgrAFsVJf3G496DgwUYeADe1iy5TtrLw6",
    "SekzTtWYZo28Kxnv5tHxcxeV8suhnM5aC6",
    "SPvvVNsFrFSFbHa4EGHmDR9G2ruiGtEQDr",
    "SeUCrZHmaDi2HX3MyKG3oDLbUNNXNpWSz7",
    "SeL5DjqjDP78PZ9fdVUrDHzhdhqrJ13nBj",
    "SMSPCCWA5MkRbuxdzGD9x9hGTDDSqUG8Z1",
    "SbvCg4q3boCg1gkpcQA8q4iyCy8wEiFPzw",
    "SNAVHLj5hTby4DGypk3v5buqbsouEiqazZ",
    "SMggVyazBcKGUZ2cFXL6HwbgmbAbG6yDzv",
    "Si3xW4WxHEox9M58mHpw256jmQCGw4oPfp",
    "SZugBhr3ionwPMDAnUs3SZj96zqBPz6kXS",
    "SQgLckeGv3zHHvuZQ6p9VfyigYTvxpgsjy",
    "SS2UiiMz9Y2BJAPjJrkuYnJebRtGoqsLZY",
    "SgLgUh146WBjkiukvEThF2oGswmiLYVrSM",
    "SaozyqNS28KFN5YH2XcF5YWXsjvHHucAHo",
    "Sa2LJM4iY18YYwMccVnn3VCs8V17p5CX5z",
    "SVZaHHsmxtGa4dPX3TUL25eWzY5ZwxUt89",
    "SdQ6bzkcyBm6jXz7nG4AMwd6vWxAChNoEL",
    "SZGibAmtJiwq9enKZqwyvX6gnshfN2PcUN",
    "SaZ1mqEDsqrqAAb1TXsT31Mn3sHbWPFxji",
    "STSK2GDohpmFkpdz77FhtDRQ7XzKxKcAdC",
    "SPWFNwWQ9RNQ6RVj5grC8j7oA5hmDWS9DP",
    "SSsJjhJ995FxE66E8sYwU3tzrNfWkPuYHR",
    "SdmcJxzD1P4fZmwUkZ8kwKBhQpmz4B1Yu5",
    "SjK4PC614dDChzLZehQS8bKHRadDaHn2Bi",
    "SeEAj7RePRXmKU4xg2t9sjK7akzwG1tfaw",
    "ShEkutAkjNJTjPFCyDGjeTak3X49DQ41xr",
    "SgdaaQ5FrAMV2RgVaj14TJfuy6ESrpEPqc",
    "STXJHV9Ns59iNqtpzstYL6xRkSNDLY7tGo",
    "SMfBekbrHapQTvVfhHFppphLGLYeZYuVRA",
    "STJQ9PfFuTkstvvtxaaCY7f8FjkG2Nskk3",
    "SdjyRq87cGiWg1a8eHgbEk1E5duMrK1QpW",
    "ShwHMXxXSLy9NnC8yb4ia3nXPKR7Hhvj8a",
    "SiWo5k6msAbdmj9GTDZWdHtRUnTjdz2jtP",
    "Sdsn6cRW6d8YVEVmKFYxLdyhcEBEE9zsib",
    "SaJeXNMUH2zk5iNsKAKx215DGYoyEGPvD1",
    "SWQ3B9oJe6fUsTiWHzEVot2q2bqofhP8t7",
    "SNiAku3mza5Wamyi3i6WQVJKM4abJBq43b",
    "SUtDC7ycuPxyBiGsob8Uc73Ni8sPP7bivG",
    "SZEvqFrF3177P1LXKjmzMSn66v9JLVj7vT",
    "Sae7nonGBf2pqRm7bPSrBCe5QegJHEH2YK",
    "SQGDBiGpCkAjw7TYRjGX7tdpfJw2KiM2Nn",
    "SSqt8zf9wNicndq2WdN3eLmY6VQUqPtifm",
    "SfcczLQpftoctxy2kg2vRo6LBdZyftmyXT",
    "SNUTUD2cgSpVXNyR1bH8xNqL9gehT3NHpK",
    "SPLGAyD9KBw4n9bTjTbpUAZ5MPBBZMPzgm",
    "SPRXCNghq6gVtPfXa1kxQQHbFLqFudmEdm",
    "STYPRXUmPdBLRiGJtVhNgFHiaGcZN818MW",
    "Sj3WEMXDJinbbWiD4iGkQahFDRT69H2vWA",
    "SXBBh4P1FYQALgADCfjzzzed4F81cGbG4g",
    "STnhHf8bS9DkvGVuHYx4qghLMCcoDgk4t2",
    "SSfXVn1Ujs7Xap9raaME8YQr3MicYQpvAA",
    "SPAYHqgTqJYB7f9kC9tcLmt1nod4VQXT7t",
    "Sb9KSN4BbU7R33KdEWNJ4vHHXycyx8vJgJ",
    "SRhxwcVVhTMU6Y8MJayguNNuFpyD7JN7aV",
    "SSKWLjr6qPFV735XpJvcz9CDtCF8fQ8M5e",
    "SYwLSy3WQrHyAxEMJgB9dD4xSM7oTt6c1k",
    "SNzKhjcmXwikpoA2NqAm7YfdjYUyDmTivV",
    "ScqhqvP4TMfEwEACccnD9w3NQaKgq8kPzS",
    "SgjRew5ZJ5HhnjtWfUVJrUzVPFqvhWqDAc",
    "STHTTdAscHRBGjXyH64FDgP8r4jGQXuWVh",
    "SQygnUFMC82mvtFM89fGRm38TDN1UPHBbb",
    "SRtkH3AttrAdojHVbYpkuURWYhGRjdUFNk",
    "SYdBrxNwgjMmyTMQKkua2gbho543ixPtd8",
    "SakF3ZPhidLcH4jcJ2WFdoJVaMFRmGGc1e",
    "SQxZLFBkNQZFMxP5bYZ2qxDL1Jt9xiSSUv",
    "SUVjTXy9qxEdhmTmXxd56VUZ4zDue6bfzE",
    "SVt1f3A6XZD3KQHg9y7ncxhGLHg4YtSyAG",
    "STbPszv5BKwMvyU6AD1CouzdVESVMenepS",
    "SNXwgFSrnYmcngZ2RM9aoXbzCBzaNGR5bC",
    "SYhrWe55YsR1NdP2NR1QSjwv5fJnSwfSu5",
    "SakssY8EY61imTH94dS3XCrLKMx3RWf7nt",
    "Sc8iRKi9deu8N1dZvEkiV2UYmuxtbLPdWx",
    "SfL4LX3iLCG9JjYpWuSufByS5osYRRVwvX",
    "SczBy7wYsubVmSrHn77431dYUEogdK8CJc",
    "SfuJyhyYEfynHmaLoR6atsc3LCQNcAkrpU",
    "SgRgXWZ5KLV747r36qMd4AgCYRTfqJ6EFA",
    "ScadgH3jumESKWKCiS6p6LkjG3KPiZcCD2",
    "SPbE3rciEYHNQUmvBoVKNxM5Hsvv6uGXhC",
    "ST89neCz71Ph5xCb2xErfsKpf4kS9G8izN",
    "SaKacnStGczSbXVDRoqVdPMdQmywVzmhTk",
    "ScbWzThb5cwbtFzSSTY6XEgNR1XaoMnKx7",
    "SakTWuUN24XJ5HmQaUFP7aSnD8uWUVgXqS",
    "SXLrtcgVb6CA5bMMiUWgvokK9H3JsdKCGX",
    "SMvJ1T43NjStNih2XbBf2XHKrfGQ7ZVqAb",
    "SbWwJX38BfdnY1NQxfSxB2aQHzNfjaNmfM",
    "Sj91WDbeFJjExr2r1Xb7NZLr5Pj6x9Fj7T",
    "SjJfWcw2jT866s8QX6r76hPuTya1qhPcwp",
    "SSHHirucgW79BWiE1guxhQULHTwQrwfvXk",
    "SZHdj4MwCQ2Hff3XXpMPzh6iS3CqRo3sCu",
    "Sb7MEAHWXd747LHKPDKM93MCZWdT1UAUVx",
    "SU4BcQmKVFSGSTRgHsggq5Xs8KmCx2MQVy",
    "SNjYqcyhQLnx3Uak7UPggv9MJpKkRCvnUT",
    "SYPfaxzKkroMBsUtRiHjyoDBy2sa5EtNz1",
    "Saeie9UwqQdp3S2xna3XdcD7mjMJ31tbUv",
    "SQ1jY1tjHD6GaLd6rFfxpyqxTa2bwnHuLT",
    "ScvmcHcWjMc8xymiXnMGmcN5bs3AKFqTnw",
    "SPAhjNobmRBc4zdiDngQgDYvJ5wQeb1mTJ",
    "SSvv4rj1iqqGtSgBB6adXgHekRnWcz3scR",
    "SbL2isRLqzwLRF6deFS8ozwWkni5Bs8S5P",
    "SUvTENGZgq1rTZ5y6GaePkTrppSF23pwHB",
    "SdDsfNgtdAwaNbX6MXw8CHE1QokRddb1kj",
    "SiSHaK2NUbw7y9umJRsjg353N97Mf6waNY",
    "ShMb4kLifwyu9HCAU1j4KKKSaMo3HpoX5H",
    "ShAbJG4xX8DY6dxP296UKzT7Mw2LvtStsR",
    "SdP7xaMQB77f6aPMVERa2sNxQVQRrZVGbY",
    "SR82xfMfygpzw88dR2AfYS2jWZTbZwzbvm",
    "SNjguUHTpp51afdw8GeFWkb1mJ6AZVTvra",
    "SQrkoufyQpDjrfCA9D9tx6C9JHziGUu9xy",
    "Sc7DXC7yGq7rTEAutHHSsUuLgcc5Dr7Wmb",
    "SbugBqLY3us4zwe2xZL5fpcyeWrnsLAhfw",
    "SNxR4MiuVhnuqVdudwgdxYaxW3KkEpCCjd",
    "SPf7ob1JwEBnbFFWm3uvYB4Z2yE9EZiNuU",
    "SdxEXBajGNDFm5AqEfzFirRs2a6e3N8H9p",
    "SVNqFSq1LioD7fUyz95tgyujy3PDSYeysv",
    "ShexCNXW78NRKY2GybfEznm49n1WL1e5dg",
    "Sfkv53bJNcQPTQnjdC747wUrbopx5fDBan",
    "SXTkK1bGo4u9oitdVYHbD5Cg8LTWwtyngm",
    "STAjuVLWWkHjnDRiC5KDSkzpHYWhhsY8d1",
    "SWeUNv5GVXrReKzF7WvooaLLGvRwRVgW6H",
    "SQbFGKRk1aRqfVm7gyebfBiXRrdkhCCWuj",
    "SSahP9E8gdqTKpjxGphFW56i4jR3bLextN",
    "Scb2fpikvcnrtr8jZcsVZuLfqFspwjqUb5",
    "SRzq3yA7gGz4XNvnKQUcE3bg7nDWMdVSqb",
    "SX3X1nLyw3ktUu4aJxEodnrtQwKuXqbiD3",
    "SR8dhPr1oGPz2M8pNAZKMEC2HYDJXBfHCT",
    "SZHoXbkv9Mq7CCywZ92sGzFcFCYe6f2AGt",
    "Sj4QCpiERQzjUfzVQhH7Dv21ZbvnhnuKBY",
    "SWLJaSDgBYsZ5ryRDLMSdZXKorZxwvQys2",
    "SedE3xR3Vi3C8RNzJq2Ht1zapZLYWoZ7DG",
    "SYMTJMXkUHZg1JviniLU9bgeJie6VZGCJm",
    "SVYc8yVDnnx9g5F31spL4dHQJ2yPirJBgB",
    "Sdxge4ZXa84VahQyCrJFoFFezWDZJk4YYq",
    "SPnkQrjhigAV3EfBCeydYUW92fUnD9DoxL",
    "SRmjETic4CN5PxrT2KeqGx3TPmevKvce8w",
    "Sc9B7fJTUn4dDvz2qgCgT7Zq9WpQ15gQbR",
    "STD1jZmWzKWmwrXwqokaRrJp7furc6Veh7",
    "SbyVUv7CHwywJ68KzfNycBjGA7Ko1yNQ8p",
    "SZnL5NrjfZpaFmZxL8211N1Aqn6PNGzTS3",
    "SeQCNRoFL57sXwreW8VztpGCEoQaM9pCYz",
    "SYRvycdQWy6Aq8cFrZwvr9jd4gH3JtSgC4",
    "SaWaSnaooCE7eeZuQeLCcGCboSn1My2ies",
    "STF4LT1FYJuMrJCzsnTEGWpkQsX8anfnD4",
    "Sc8vWiXeo9DQeLH8op82BEDZYMkjeyV6CD",
    "STqHeGR3xiXsG1mmsQzNBcy6iuCXHizuup",
    "SioAtSPuxEWbnz5pqQhKMqdTtZGT2bhXR8",
    "SkXiMFMB43MMUCaD51BgvQACmfnuHyQRiK",
    "SiqawrJpieQxSCoKDZ8mHyK9D92MTP82B9",
    "SNe7Q8KoKLTwcT7TtDEMBfA4BB7dWUQybJ",
    "SWqLhTtmGczwTyTBXkoPABrs7kyVbPCVqu",
    "Sh32wTwCpwHeAw8GfDExenbmEJH98L4RvP",
    "SfgsxJw31mD8nTazELqwn6dH1pmuASf3tJ",
    "SWzmHCJuGTmJAm42RGJMqNEpRi6q46fJQP",
    "SaFefWrWLAAoGDVbuwQUHq6M4Jq39izthA",
    "Sb986QEqwEuTzVCE6f4s8cxgZF3KPZ2Ls8",
    "SihsC9GtGcqZ8XbZ9pWZXUcAWpGkTLNdy6",
    "SdANwKDyM4tjmSnJJtzf7PTNfnM1H2rc9t",
    "Sjbj9oyCkxfWuLM2jc4BxnwdnJfHHtgaiT",
    "SdT8gg6D5Wd6QD8on9VA8A52gVFqeQt6fx",
    "Se8EGh6tjueirXhddzHh8YqU1JJDbdhttT",
    "SMdkurtdLdkwdrEukrSF46e3L7fEjYViAg",
    "SQRVM4WQ6sLatnSm6PcaD12v9F1TdJSwPk",
    "SMvnMw1Xb99n1LCJEBT21URJbiGJkwiPUs",
    "SZK5yE3LTUTo1PTYnUoE3aiMekUdVhUJrU",
    "SYED9e9Ccc99PyxwMo4HtM9czdSRbqM3wA",
    "Sif1V7CviNVVXz1xVVsJwjcU3CmaYSrc5w",
    "SY1Jh9mcbZG5R2y3i6pBcGiURTLkLMZYwS",
    "SZYxpCWmJcFQRobkpx2Zw668fHzxauUQ6i",
    "SUJtExZtfcC5W39SBKYcc1b2xb2GWP4Cww",
    "SP697ENiFkB7z6RRdFr82kAePnzeC8UeEh",
    "SNgNe9BnjM3M4Jq4tSVUCNxxsB7pw3EZH8",
    "SXmEZ1mimLKUtqzQRfDup6m1LgsNTV9J7c",
    "SMtbpkGLAGBL96Jad5KfsETtsMy3H64hZ6",
    "SkM69bkKtYSUdxdXpirUpetuXedkpEdVTW",
    "SaCyg5pzoJAeQqnff5E57NqcUbV4TJ2UxC",
    "SUvVng1Y4y5VHEESa1yokoub4Mjy6dywF3",
    "SdLKUjQN3i3tdshyrpLsZ8VCGvp6WUEb6m",
    "SVvhh4PiSjYqgj3T13ZDqnh15bnJPCGUCp",
    "SNAJ5uaEz4DrgrkFVjoyokF6cgBjHTEALa",
    "SgpX8MfjYow7b3qjS54vNDC6TnRsTTGqPX",
    "SNjN4UrrQTUtaMbFDE4EZeKtJ5611EST52",
    "SZjGcd3XUFPDkaoG5sS5eYxQGnDaw9DG9K",
    "SdqoBxWorrsNAZgjobyJB9m1p6hN1GhRSc",
    "SW2TLpvYT8m8pwC63yybZFgH5QHf7yXcG5",
    "SYDkQyi75vBsHyj2r55tLxSFPq8VuiyWca",
    "SecPdyK1bC139GJzc17NKtBzXxXLAFfK6w",
    "SbGJoM6Ha7N1FvskYxhmjVqJC5bi5fXm6y",
    "Sgh1wbtU5NmmuhHma7zzjZ4akNtuAZr7bt",
    "SQk3YEWDmv2GybHWUCzP2iqW9wfSLNqsrU",
    "ShoGqsQrVB4vB3G5JwTcdVBo8G9kpLqXrx",
    "SiVvvP4rVXakvxYQCZ3tNZGN9WTqeqWWdb",
    "SdhiidXi2sTZugXw1NepRgudajGu9covtT",
    "SX5dc7Pj39kmvmz9FTrTvdFXf5Rp6yWvMW",
    "SfHZs78B9ekPK5koiGW1kEACzGGk1cq5VH",
    "Sc2Rg8jCwnBLk3xH59ky29XZCDqgmD6Gde",
    "SQ8E5NvJhCrUtwfiXKGPsCuM5FJHUfTXxQ",
    "SZ5wkMCEfreJVExwoD7PG9bCNg9BYCuWA7",
    "SWMVv386ewWqjBiKn3Av7HNB4WHLq6BxyU",
    "Shvm5hkrT67dQMzL59S9AbwH3FQDkRJDHB",
    "ShX5S7xeJFJZLGUYrZxQG5CwR7TqQn2i69",
    "Sb8qb928fTe7XzDbxY4pqoKprCY6QWW7Qg",
    "Sau2R1ipXHvjiysLSNEsEcstzMbT9BtQ91",
    "SRfnkdkF5WofCpnsksQWgMFxwuvucJVAfT",
    "SjArLTUqKFSygvVNYH7Ndt9TVVxNkLb4ev",
    "SewsGcg1Ac3WZbiQDgSbPe6Jbt4j7ERajB",
    "SQbUm3bishkVsxjSJDPYDbfEKrTT5JLQao",
    "SXpf9CHJmRypf3H1bkr916RoFpGbnLkUef",
    "SPz36p9fsfkZ7mmcepgzktPHhRetsVVeer",
    "SggHJdiJbqQQBANTyAaTNLcJdUYX6iFSTK",
    "SUY13tiFiAdCT3V242mypMQeqxByW6U7n2",
    "SWibvdpqAzHg9qvNRcq3VGqyU7kyMNtDKm",
    "SXonx7rtwwy2imuS6C7HYFfowEfXSKN7Yf",
    "SXQShoXkrqkDHTCmpKM43XfxgHFSPzsK1V",
    "Sk4SLhpqnLMFeiwjVrroXFxfhqs53nVG9u",
    "SZ9twugTHyrVbeLuS9ant3ojq8cQ3uNigJ",
    "SVWqaDoTkGcXvtFY2M1MeUxYG11Fwrox89",
    "SStnHDCNtmCMbkrLyxzMAsgMZPfZgnyZrf",
    "SiWGV4RBCLN7FQpG6QzDTDKEk6vaPKd8af",
    "SNHxJvSDqxAnsowaT3B8BadmHgFamQsyyu",
    "SdDLdMCcFP6RWMkb6wsp1uDDY3rod7PFPa",
    "SPe81zF6S1J1GFLiDptrSvvSMExJyifKW9",
    "SQz3PQnV1mEH8rhLV3gbes77JtuL5YPpQB",
    "SVtEFLQ3JsnedaA85XpEpS8bDkgMmcRfhk",
    "SU1ci9gWSEt9V135SqAmUdN7T2gQxkVVoa",
    "SPadiiTFuY6nh81Ubj6sNj6P3YprjNLiBy",
    "SPMo4Att2yLohitWqMXcXbWGDi1d4ahggu",
    "ScCE2XcYoh2swtBfNeu3jZ4q6QGx76ro2v",
    "SaYzn5i3Tu94x8Qrt6zMaSoWxiHX8Z2Wvv",
    "SU2i6nJKE3SYWcfsMp2QYnbiQAnpT9CsNU",
    "ST4ZzfvgvLyRhSHCMKgX4BeC7u8gobpoGc",
    "SdT4aRrXY4gvFvcugGAo8aQNinwpE9YEXB",
    "SXVh5nafxHxgeVmd7jyz2mqYhHwNRtsDgL",
    "SeKyvmRBWa7FqYwQvvnp5D8arspz9v5nH2",
    "SZjD9H62gjzYAqRjsgpJLYqRJ1St4LtLby",
    "Sk7h546scHLrC7xm7royVhF3Lm82MznAzy",
    "SjNtFLdMy9YcRFbfXBYCSfsN2Ejo3jXK7Y",
    "SSFQvrPvzSjQz23WVbAHguRqMTDPU3Y52F",
    "SN9h9jd7Ybgka7xpmaEs9jj3iHugbdTLLr",
    "SUzTAzwtyij9TLXfc9EKZ4MTw99bfFfzXX",
    "SZmuBKNZWBUynbYN4NeYooMijpcboers3C",
    "SNmTRWDH7ZrXSViHySDCowbohGijjLwB4d",
    "SW3seKFizB5dpPRiAAaEJEfCNdGrQANtTs",
    "ScpsktCJNVCHfHdp5xJWuaFRhbzFohir4T",
    "SU5ATpfU74tm2J6CQJ5ZZMnJkY6HaHMaip",
    "SZgoQChQv1vmCmiRSCp16w6nrpYTKhNURD",
    "SQbWbu6WC1SzshKLUf6CYy1CXpXE2wrtH3",
    "Se84FZDDC4vmtpkScNrYnEfudHJKDBuyW7",
    "SN9u6g6nuADVKfBgWNL3PgaAWDDviL7Hir",
    "SfoYMipaRyTbYaohZjeTBugLU2UxT419Cv",
    "SSts3Dmtt8VBXRDmZqzS5oWvDmZdKSaw3F",
    "SU9NjwBb4U3ycJfMMh4KMibc4zghSoo53U",
    "SY2UGG9ZzegfkKdJSg1FmZUR86S4gYSiEw",
    "SRiwipcgrQoDhPHXmFyCCuhxSiRnsYqrYn",
    "SU3f5S6LRhnmtEfQYJvkgYbPeZ5psFtzx2",
    "SUgkY7dhHr8RVLHEBJqGmoBBsRYM82jMHT",
    "SPM9xqBijS1GLiMfCAnXN1sWZqt45XYBZc",
    "SZmKPAhTqXC481gxBgifpJNzP4rAmvZkLV",
    "Sf5a1YHLWZrNF8LfBwmBqBTdPGQa1oSXBX",
    "SQBA7RKjoVBbXX6pouyzpKvDJQr8eQjvXh",
    "SU7oZ1yVxLAEoT8XyK6YCrLCojjCAFn82o",
    "ScLMgGemBLsK5CLWRnbRorLqAwjH6DZ9hv",
    "SfGB7PJxYSYoHKgeJrJKRk1cYMTzp2mNwf",
    "SWh3EBiRxABjm9eJuGgLNydA39BMDcYLC9",
    "SWUwohAb5zbQUFSxQrsjsKsH1VVknfaDNg",
    "SRCDCrcbYdmnqJ6Aa6GXXNDhxbYEQXMWkF",
    "SMQE4JV9bYSVoRnu6nWiDDPtUhxHeVLeTt",
    "SNi5Jdwt6imxh8snsMMwqatwDasDorZqkp",
    "SPx7WKBaDxLgZQrzHVVMGRLBh4w6p4xCJR",
    "SgeDxKc6r9Bq8koYDz5oQDu9RyM1NL6RCF",
    "SVhjnn5suquxMmzGFFsiLvNpBaTGuJnr77",
    "Sj9kNKPBR9wCAB2xM8qMxyJa1YqcQi9sQR",
    "SSEshQJGbisr6DpwdYPDaELF5QV2fi4WjL",
    "SQMLpPQsnTzmGwD4SkQUF8yiQjdSozKgAG",
    "SRm7rRhkznQDf1erCLbt6hNG5TcSbkqcte",
    "SPEtuSYgedMtNLrbt7nxq3qQd6LNtM3AdW",
    "SSVrsov1SFpMhrMuLEUfXqYbBzjnC1kUmu",
    "Scx1Y6aj7GVWRP8df6moqnnfevYZY1ChyQ",
    "SZzkL1SQX5hC5V2sPDDhvbcwXSSF7Av9BL",
    "Sfa1915PE5KwTRoL1frDZddnfFBqJ2bnPr",
    "SNZR2Zn9i9grV9MWDj6ggbyQtrosKZ8iTk",
    "SbHbR5Jn65Dfdir2aCCQMTKaSG3297qgoe",
    "Sg2HxxkhG3yRzHW7NqUKrnUPotmU58HdRG",
    "SiqiGnCTBo45rPKcnxHCT5RCPuStQB2Jng",
    "SW78vUm7gcziFtxo8jivNbWGSgvVU8QGLK",
    "SUT8uvmBxT6ty746shnVnyisrb1Zv44XGm",
    "SjM2b2ygpFFUoqzsED9cNcqdj54hWaXt4r",
    "SS2MbU2fZucxZ5QmVBZ6xtn8jcvFTmkat9",
    "SSyaHvfZs5A91sYoxXhpnvV9EcARiYtHVY",
    "SkF9MnXWFMoHm474xkHXDM6Xi9gBj8SjgZ",
    "SW4r8a28x8YJQQ1XUfhfpagm9bmeisBnkY",
    "SeWMbykQtLw1jJEa6efW1gzG1ri5KVcJ89",
    "SdXM8gXb5KBe6uoS7RZ3ura2suGu37647E",
    "ShkWNFxFzsrjiXWpcko9PdMXkvxavfALgL",
    "SaLbrQDha3acEYScZb59KS3j3XPqf5FbG2",
    "SabFTdHeYM9o2fed9RgpAHcMuKC9Ymp2Kn",
    "SciEDniCjPKgvQpYgWbhyuLnzzroZmpWiF",
    "SSiZbouLeFPUc2C9NzQ1YNMQwntQygaJSy",
    "Sd2MsFt18EmvLHpTuaWaegmebrfsdA4UzH",
    "SVUBVFT3EkmFqbyoZ1FFLHaNv13oaawJ5M",
    "Sb6K7GCJ8RGGXqeLzw11rDMvymB4Qtce7J",
    "SWJh8kJfpZBLoJVaManVyEZjT5tiTd9FSZ",
    "ScSryqsTEovkEiLuymyFueCCj2hScwvckZ",
    "SabstMJSiJhH4fPwnAjMbwZD93hTeVktFQ",
    "SURfRADz3QNChLCNYXTiW8xHyMqt8tEps3",
    "SMJ4ez4bZkfVocCqX8vnT9f2uqofXMuuHX",
    "SdsbimBy886VKVbFNcB8L8nqvBLtNMenFP",
    "SiRMd9wGiKcwnTR4FxPkLxfoBT5t6bwdPo",
    "SkPxPr7QQLctYRmRqwJt4jjVy5tefxN7Zv",
    "SYjHD8QQiNJjjeeMK4A4DgeziXoM9Vfweo",
    "SQ22adpZ32BBHJ8bVZGj5ULadiqZVmdRTi",
    "SMShPTnipiCtdh7L7mTTXAwBggquJRcUuY",
    "SSp6F46i7Y5ZGK5BB9XLjGzCiXWhYZGeK1",
    "SXApdtbMWBWoXJYMLQzoWhjm3EQSCRW9h7",
    "SNJQDhF7PGRCyeezwX46ctwZhWBbr6CP1w",
    "SPQ8FH13jKoVWc5UHdx2aVKbSbu4gwnbd3",
    "SMpKXqjhFjdZsb8Q8rb8XiJkxuSUKJr2sV",
    "SRKLpRgiCMGLGytoCBYFZE8eTUGY4ez85a",
    "SaqDXdxrftbwbFyjpbJwUGaiSuGoAE1N58",
    "SQoPwXRv4ivSSLi5WVp5ywg2yNST8NArX8",
    "SgLLXTQACo9w6BbKZjTEw3NngJ2EKbLhqq",
    "Sg6TK9oKJ5HCHvmbjNczxb8XJdqN3tzuNK",
    "SitL7ifS9GA5iZhJYm3wiwaF4QeBs8gE5N",
    "Ses9Ctbbeb9daM6qQTjs3qfa4APQqFAAyw",
    "Sc8SwPQWRZuX74CphezFPQHDnEX33Zof4H",
    "SSGE5fvX62pP1XdjjydHQNEq5Agrfijy6L",
    "SNQrJ3RGUsW3YkKXKDALJZke1g7xSkrLRw",
    "SbVKm1bVfhC9AbFoPAXh15MSfjZtqfLPDt",
    "ScxA7LyQzbCE6wyEp1AnoQxzcCboXrcZum",
    "Scf56YSp8WJ8DJfg22nyki6VvAPDJUaWTT",
    "SZwFjoDmQZnuQ5qzegnwxFxvG6sJ78vSdb",
    "SQ2kRXwjMzxUjzgChjHVpYuPq8EesyGeJJ",
    "SWo8BP6kH6BXZVkX1v9bwZZak4kkjbdVcg",
    "SZnjUnPZZPHiZMXJJnMvmaoBr9jYHeVNep",
    "SfoZvaATUBKFkebtk97CBGTUuj2FWxnK8f",
    "ShV3WtG6tL9B47pPCxDBELecCPvdz2dGcD",
    "Sb2CHWbFsgZPNGMXrGqMrusG8teDmUcLrB",
    "SW73gHFBnQZc2bzq48wPdRYryLNfi23e4E",
    "ShUVzWTERkrTGTy9tx1ghQjFeLS8cbhbHt",
    "SWoP3jKi61VrfPSMUFHouTNKu8G8HkWiuN",
    "SXiBh1YXzSoSKenv1XuUPNPCt123K56RAc",
    "SkMQB8634yYcc1GLQCdoaErqaEWxin7q1m",
    "SPTrF9NiXSLAM3NdSur2XQZaFhkbYqmfcB",
    "SYhkxcZKQvDgi3tNfr5dAEyqjyXiBFKdya",
    "STMy2tu4X8WEjoD3Lt5KwXW9XKkx8wa53S",
    "SXCW6VkuYxedEy2g2ic7ctSRiU5nqvpQ1G",
    "SfxrkSnHzQjLvuzzffNfi2GaFtBc9s6Dq4",
    "ShxPHxov5zwtYW1VBNaDMzjx6CYpjBwxxA",
    "SXLFzzBx2HeAiTuLooMpby7uHdWYJ7kkGk",
    "Sah3Ku4gxvjBMAnkrRmrkhynuvqSWW5TK5",
    "ShnzN9rYjLnHoBFVcLngUoQSiKfK4GwpCp",
    "SkK5gMosEZ38ienrWHm1y3fdCxK5K8Jja6",
    "SZ1RkgapiESbq1w65rHKRobRhJF57StJMD",
    "Sfo884NW8UmdyGzW4Qii6m3mpfzfBT8f7C",
    "SU6JGDoCRjge9Z3m1NF8SbrHtFJcxqEHQs",
    "SYEKTmuF8Dgd2pULbfC3MWj6jZUBWQ5n9x",
    "SXiPBmYPqzMbNSVVg8d94V8oPQooQT7Gso",
    "SjVK5UEtPdY332KtppLYFPcoDLVWhTwJjc",
    "SYoKcbMaGxKwdc5DAbxQYAp15fD7fphSjx",
    "Si6qXoqLS7Cca42GF1ouGJQE5vhdGXJss3",
    "SkcFJt2CD1bLvgFTJJFdxtj3uawfBmh9LT",
    "ShjigBisnWBCGGpPqbExCbmDXrL9uNV2YD",
    "Shcr9a7HTrDZGJCaxgvKgJQPRU6dc4dp8G",
    "SPLz55SySne1W1UX9QHBcpQQdNgvJkiNS5",
    "SkGMEBXURFsuT8op9KiyNpd7xJ3D1Zd2kc",
    "SUE26rgc4Hbfm1SAuA8XXctGrHF3xZNuwE",
    "SVu1JGbPjmPTGyZN6witp6hDdgnavTqFhb",
    "ShYaJHmXeKLLUjRrKNTLZc3qy3kLGD9tbe",
    "SRQbJjGLM3TSJXj7pYtcTtYbYRzcz34JRG",
    "Sco8BgHNA72L5mmJuEjrfj2eSyLbuvPpwy",
    "SNRbGWDq6quAS9GQG8cL1xLB4eTmESnpEL",
    "SWdJXFbDQLDgcw38EhbBxxJgjXNScjZZu9",
    "SXFc4FygHdT9GYBGcwkcT3mSECcUo6rUmg",
    "Sgptvutvk4bDZ9PHnLj6GjxCrk1xAoDFan",
    "Sd4qBqf7TLwYsFP45VB3CkjyQNEQswYAKT",
    "SNXkHURrrbBuVHcxsRVn8r1EYPAKvUx2vH",
    "SPBx9KnW9wmaUh8HyeS3Ut4XjXBqYGeHKa",
    "STGJDzSCPPHfZLGCRPa6PgQ4AuhsxQGA5e",
    "SSQ2LTbjCnUTHYRFGSF8wJUq69qEMXiCmd",
    "SNoxSW5XqxWRs5Yp63siXGvtakeWymnhHw",
    "Saabo5wmpBrbf954hRF7MDNeymnFn16rSQ",
    "SbeTLMFU6egwfjT6Vwm2LKoDiYB8f9gikk",
    "SaJvDN2gBADwUXtmhJM8k8NsFNUYUM8GcV",
    "SSutN98sFAgfcgbstZZ4rUzSMtszFAmNoB",
    "SYz2exLAQpkGxXY8Cakyfgk6QTxCJKxPVP",
    "ST84ArYXXnew2JNnc78dwWRSjZ3uwZmyJk",
    "STNYZY5ztMYcUDUKDyxUQfEcJUx9dgF9Yw",
    "SZ6pFa3TCF1Qqe9Keam52SY2fe41Mv2iAS",
    "SW6f7J62xA89VypLM9bKQpDi3m1YbekTbq",
    "Se2eNieGV9nNGALzMuzaEyHKTwCWxTxspT",
    "SXWoo4E6YLD7XrhT3UZGfPZEyBpQ1yG4DS",
    "Sekzh4F3McmmhgDYgtKTsTiDtkSWWmVLbA",
    "SQnwVzHHqKfDJ1n2TPyMsqUtsbnM2p6vA9",
    "Sied5DmCHDLYDkxjbmg1zW7VVUAVGXK83u",
    "SgagkAoVyw65HQ5Mo3hGjeByB2LVXansgv",
    "ShQDj7m6WE7RRpK76h6Cbdve6U5Y5XwNWi",
    "SWm9DNEDQk4rJ92eK6LwR6novocxS3cb2v",
    "SfC9hLVgTkLy1TqmAoUKUQfFjqrM6Gqkx3",
    "SaLYrJz3CPtt3CFT91JiLEZnXE9amhTaq9",
    "SjXRuDtTfBaRKHWKQSvWU6tHJRCZ3qRn7Q",
    "SRzyvFJjWz1CvMHgzk8c7cRamXMy7Pn2MZ",
    "SZcEEcx3FuoVeKDF6wxqKPiXPVavmGGSEk",
    "SfYbs71pYHWhePnrGUVxhCBQupnYPbuWTU",
    "SQvvQgFighP7m2dFzGsyZtqEWwecjrS1Vo",
    "SVoUkgM67zYyHJKhy2wHUcU5pk9NBC3vJg",
    "SSqTN2b1FCZs3GdRTi9vjPVRGPVo21XHyn",
    "SSGUGw2iMS6UgdorqEihLHCJoivhX2d5oq",
    "SQM4k7vmqKrnw3bS6Z4n4xxV3f3ny4rTTN",
    "SdNNUctmNvTYMZ8bC3qax4jwj4TgzbWAHC",
    "SRVEQh78wFdz6GVYwV62B7TA6pYnw2wmV2",
    "SaNBroKNyLEK6fi4H28d3ixQqw18FSujyF",
    "SWYws9Q4yVnscfmiXPALAH8T9VZWZTRMCv",
    "SaqfsKnMQcWMMaRAvbg5Rq7MMUQPt4x5hF",
    "SgUv6GQmWV77tHnxri2ptAaye79uERN3jM",
    "STKLmqMhynwKjDsJJExp5DdVSFMgunNVMS",
    "Sd6ceZQirhCDRG5CUVqhGqvhkJkhL9fTjZ",
    "ShdZBAenoHSTw6C11XFFHDLxF6VmhSQBP2",
    "SXesqmgYtRCt631hFumheM5X9rYmXgcM6U",
    "SceebADA85W4vuULFzKephrWCvi9gxKrVh",
    "SPJcCmRyyTZgvAhTJtbpS7V3F9SGaWRKA7",
    "SYv6Rite1DHNuPFRCDMpyK5met5oQUbiHu"
    "ScdmXgnDRUUSZvw7D53EyX7mLGo2YJowKC",
    "SiBC5pVysUUkckqzVKfUewNDVEhkncZC3Z",
    "ShLYQDheVGvpcyMRhsj5xJ1bo4w4jpsWLd",
    "ShrGGD6nvtbp66jHxWDeRVL2Mxf7wFuPme",
    "SVEy9XMK6VvxBSLE9hgQpq9nMaLsjbDirz",
    "SPZQYEZQ2Ys4bpXQji6a4LhDxF1AELLJNp",
    "SWyM3q2cnW7PFb84AbethNRidZ6zyvAjr6",
    "SZqTBJ3UQZ1zQzqwK6Vb463nkWZWKUYKMb",
    "SdLpfkohEfU5MhrM6cpwo7pEQDSJqzBkMi",
    "SNxBzbrwx58dEGxjQBvKtehxMYRiLhp3bb",
    "SedH6Qw2qj1FpUxcYWupGY5eiKcAXXEZ72",
    "Sb41EDpocHEp1s9ajNEpneEnkbz71jzeSe",
    "SeZETvLP1gC2EkiZU6C8RpwNT6ddZHcGSL",
    "SY4AZWD1RqALGyfnPPvUUfv8Zkcm5eFoTG",
    "SZGuhDU1zwL9Kctvex9P2hqnypFmsWHDYW",
    "SjAwHgoZjWEULXgKcCmyfJyU6jWy5QSuXH",
    "SNwEjY9NcYKvAPCF1B8srSV8wL9NowbVDY",
    "Sd3AL5VtCQ4Uat8X7jdbY3eBVW5wg94Btd",
    "SV2ZCSPaj9rQ2NLnJw3FymgopsNN3a4WEz",
    "ScyBoaq7PRf2afdRSSDD9CzxzTQ85GJ4KN",
    "SXjNRcPK9Tpivm7q6CfGHAE8ELCDMrhJqq",
    "ShsafG1T1niMc6xTiBm8xnRAgaPo8rt1fr",
    "ShVs57QyofY4KTpM5Q9jkGvw87GwbwuxC2",
    "SPEVbv1iPsXsfr9abrnhWQJAa5HdzuSwuu",
    "ScQVHSZbqA466GNuzajZXU92eZjdjGYF4f",
    "SZXz8nSqdJXt6JZoCnorXRFPbRzjj3k9Et",
    "SWYpJrriwU1WFAV8ABWkHjubjHMLsGLZLv",
    "STGC5NCkUSga5iQFfPJiwNWuj9GJvW6qBh",
    "SeGepD5PkxAQPpHKhr3nYRAXTwicD8RKbJ",
    "SYovhm8Br89Rzx3P3Aq1kXWbxsKoc8E5jJ",
    "SjcSnV73XqPDvMzgiP3VVytZzZuhcSmhyo",
    "Se8eXZ2XQYinDyaFfRT5dSEfs8vtAFnvjL",
    "SVc8u7pNdG6MCT1YnaP8sx4xrYTobNyFgC",
    "SWSq1ZhbAnCdFudKpHYzoGGk9gHr4QhpPj",
    "SUn3W5KGCFN45b69K8SMuKpNVphpe26r6V",
    "Si2ng8xeqB65o3ipsWaw8xZ8V3fmCURDgn",
    "SiSsYG7nisDqRDgP1gCw7tZHfUJa8NrsGq",
    "Sas9cJAneDY5hGCyHj8BrapaZFxTmpjjPx",
    "SViUz27UHwNUnNqSdF4GjPSB4z2sQ7tk51",
    "SeJzL8WPmDL7jZfHyajSz6QHYw6nZQzVRp",
    "SjKC46yUBqhgZgUK7fDxjNSphNZd8DX79t",
    "Se2vzUFc5yQ2YzTVcyBetRRPNvXzN7dkxA",
    "Skbp3hYgS1MDH23xUCHcyGt4s2p7Rh7i31",
    "Sf9nD7CpNSU3tXgq33oi5fxm26jyfKyaku",
    "SjG47VrZgdASkCYiVrM7JtWedkTYmsjbe5",
    "SbLeJQYWWthFB8dqZJEwPMx4ZkxGLafFxd",
    "SUqp3UZgofemuSBWFWN9zGhTJ6A5JaYBg9",
    "SfGw4MquyeHqeCNE96jrWXNs3MoRKeVirF",
    "SdbzrdoTwioEmBjUxouXCkjvnGPF4DSnFr",
    "SXR1HGvCNG7ip22px1zgtAaWT6GZGZHzjT",
    "Sf2yoScrL2J1sa9U5pAVeTq14XLsG8H7w1",
    "SdhaBCEAw3MZoe1EcftSbvbuUL7CrMZSNR",
    "SZte6QNZsWQvtt3MivXeHB91KiMja4cGkN",
    "SPmXKt8C9yrzuTL69Ev51Jwkm2DzTrFcqM",
    "SZCKnS6JyKAf3Ji9xQGBujcvxnYEovAxnj",
    "SPSbdto7ajaCmhJyPDs7Q7BbxJy8UrDGqb",
    "SVwGoEkTi8NR7MuUbnKUv857YM5VGR4gdZ",
    "Sk5SQgeBHd3RP3Px3V9e2wU4xarJYmcME1",
    "SSpE7PCTkF4NJ7VLNweBg9fUR7NWowhNmt",
    "SdfmikxWa6mv6rWhKWqmFgUS25WH8Yrp76",
    "Sacg6vmdVmtW2Fk5Q4XJ1r1uFLZXAyMu79",
    "SXLNsVFcabp8vXEkdmZn977JLEzuYbjexE",
    "SgJdiVqXXBq3yLjvd79A3vyrbBootB8HvC",
    "SejbRDYeb4VTbfSrtvixsVWisdkY2XE5JM",
    "SR5VinCdyJ2NaSNh1uL42pw9e2kuvGXceE",
    "SZPaDEKiymA2mPZ4dzBegSdFGXs7ijV6bW",
    "SYJwHm1ZvhxyiJvJozGHXGh6ygZSxxS5mj",
    "SYeTgdBtUfhuW3kpDubLPCVC9sd9mkmvk3",
    "SbHWjTQDontqETeAibfTSn554yBMyMCzgD",
    "SZ4xpy3J9fsCEzQZ5ZSKJ47ycCSYUqmnWn",
    "SdGijcwqmSDXY2q3BHuvGB3SmdPCof2me2",
    "SWHCaMCWrQmmhbcDu24kA3vJmKpTUqX9eV",
    "SRs8m7XchV9mBLpYiAeUcVN4daRxvboBTK",
    "SZLcZsniPZBzazy2aBkNvQA8MAijfRzXZ7",
    "SYEXSSUUQdLmnqNQZFCdBf2VHGfDZ2PRA8",
    "SV2Sq3EJLXCUixZhMosTXvfRYbkqGKWAxg",
    "SXw6tdHUmcFD8cVg1Chr92EPuUBVWECaQ1",
    "SW2G6VAU8U2BcBVPZTBhBEwjaGXmCTE2kb",
    "ShfDuVoSTbKPb24DaVVw3r7pwC9rKKpM2L",
    "SjtMCe66U9bhtnV8YjRMJKPAjbQ5qCSgKb",
    "Six5uH529AbUEru1XYKpAJztLYoax77YkP",
    "SW4hhJzFSaTuiHPoPp4nZVpDJAwhffcwhx",
    "SjQMZaFfLic5fMsum9siTSfcqNYFWTEnqD",
    "SjSJ14DDJsfjoqz5ggRkXfYzqNLND1pzEv",
    "SQ7RUpf2ZKFacx1JBGMe9EDMPgbLKvWEpa",
    "SXScpHmfYRY6W91sDvJhzmxrH5L7RJ4A73",
    "SZD64cVDjq6PcFkc9Vtd9XeFrw1dUgw5NV",
    "Shs488psp3JoecYXwv4T37odwEzLaQ3UhN",
    "SXTvxpFX9M51L6uhi3BUpCQfvGWg7okjJG",
    "SiKCRErwdTR1PPutWYSMvMntxZMkDAE2BM",
    "SaL8WirZgGRUYCfqhga8UPpvwGJnzyzfn1",
    "SiQqm5P6TSm4Zyc8igAZMamXFEpAB7CMo8",
    "STEZAKV6Upf87XukvtZiGoA5R5MVejnw7s",
    "ScPsSa89mFGa3gukqNnLSeFBq5h3e5xqB1",
    "SixgWuotpcmgBqZyJvhkeTmqHptpu8TfKU",
    "SbEoHzETpFk1WuhSkNwbcYcARzCPQqu1VB",
    "SQkis2s7aWJnLkj4yJNKVGCwCtHmZdrTur",
    "SfN7BNUSaSASWmwtcpwb5dYgDvcvsJSBFD",
    "SbGZhz7xZeGYiQMs2uQ5JKhxiWoQ7dsm6V",
    "SaKtcx2RnFXU1HSEuDiPTLcupwtDsVEzK9",
    "SSMMnrWDFNXCKv6UrBXvkNRN6Em4fwSr2w",
    "SSLwkBJEcTvBAUP5Yxa6X5kRDwCHzDbCG8",
    "Sc13SqPCXmga2Wzjx3aZDVwcJyz8Ju3jLg",
    "Sd3Qzkxz8HjGToCZ748TiCKEKMtmZWHpZa",
    "SWMPEJPiFZctUAiAX6qQFQqmdwcHPFfpQ6",
    "SWmyRpD8wMJXVjkAxXbSAdHGaft4BJkC71",
    "Saqn2HnFwAFdTe6zj8uMSe6cfYhbngHQ9F",
    "SXihZ75qVJV8CBpZPhbxaFFnpfJXTrxQRW",
    "SVjDiXabkvvVUSK6EWssxjroRZWkcYQmw5",
    "Sc4hetMEEyxgXoBbGiAmzFNdcEJDptJX9w",
    "SU6SELk4jcZu8aD3wHEYt4bSx1YZPQmC3d",
    "SdvhzxBeiuCUupd97U3qPfPNX5maC54RMh",
    "SZD6q4vgKXnwmwpewcoa6MMpCosTXi7Q87",
    "STpGLXLWgnAVS3eAyc2AqTDRU4s4mPFQKJ",
    "SW7EhRQsazTUnFhNsq1Q4JsCrotnE4QU2T",
    "SS9VNKpbLjLoZBhmSQdj2NgDXdweo4CCfD",
    "SRGgLQunsVz6yvYFBxuLxSEiryLpNv2CZg",
    "Sdsay8ntkktCzwwpbZJAL5Tf6nMwtmGSrv",
    "SkDG4v6zErJpPZWEKzG64FojspRhYLYFC6",
    "SiLuee1FQJLRMJuJ7uZL6P1db1ATxP5JYc",
    "SWGLdSmyTnVRY1xNTR7zg1KKv1ymVgSEHW",
    "SdmZfw3QZKEUFGh3GBfpSvLCdYHQtdG7n6",
    "SRuifzJD6Y51A6pMpNFfqQ4ZnHuE3cSz2f",
    "Se9MjJ9pkQNpkANytHeFrvxQUuQFcofUUk",
    "SjfsidssEkgPzQMd7Pg5yZ2WAwSX9vW2AQ",
    "SY5mEK3s5kopwC9jSkdp2RN3BFVGfjMx2Q",
    "SkB4gcxVb6sZBTh7igi2r1SDyzznQ5zqhd",
    "ScXpxa4GwevJ4cYjkZdakNyTPGqxk7raor",
    "Sgzk9AFkFuygSRxzYRegZzee3D3mgDyhoK",
    "SUCnaHwtZ4TJhxySBsAG1yhF5SmCS8jahx",
    "SSCJbcAcb3r4uiqSSKw1bU2H9Ys5Cr1pNt",
    "SS3ec8FG3pjmMc488VhtAiR11XX18tWDmU",
    "SSWVdyZKqQ9qr4ApkXjScyxVqX6nofAv9N",
    "SbRmPyPiNKWMHbt6Kti7eRUqnxCiSu3bsP",
    "SUXvjdp4MnSfjhPB4B7h1PYhcZbnyDPtrM",
    "SaJvm59LprYD7vPBNfnkHadmPDgMdqnVdd",
    "SfgHFeN3uMRXR1coNiodMtBJk1ZT6LNLMc",
    "SVJaynNBQqRy1EZ3vx2AeYfu53iYqdhTjp",
    "SbJa4MMoBqVZ3CN1aw3dzegjmgKjjbC9CF",
    "SQ2hLV9o6Bbo1GFf828LaxS5q5dUXTV3MU",
    "SYpJt8m8fan9E7GSjy1pLmRttFJ95JBS28",
    "SiksrbNsxQDCp3TsqHRRQeAU99NDZCFbMq",
    "STVxUdXitecDM5MmyrrzHxLguuPCFqktb4",
    "SaQLrBVcnHnmQQE2WkgDrHbwah4iQ2X8r9",
    "SS7CEtqZrhFV6qhMjo8PxLoipnNACaTtD2",
    "SV54u6yhfrgzpoz1h73Aa6D3qMEAqzoVP8",
    "SUiSccR7xSKKivMv9AHvLqQ8gi3Msgankr",
    "SfN8Zoq4W3spi7CDgpBES1oeHZDt5kYf8a",
    "Seab6P6GWcQbtmacZRU11z6xqGuTEujPXs",
    "SXuJePRMqKXnxgCgghN2CbnZZQ9mPqR9XZ",
    "SUcU47CUg94sqSjrASwY1yjfBvNbSjHDM6",
    "SV7ojfZ4aqWT8ea8As1FPnS4nSg9xNPTYt",
    "SYXmwoy7Twvb6aVDz9uJRke9veLMHnQM6z",
    "SSkZyQqFMj1auUTSCSJxnoTZ2HDXNhXddD",
    "Si8uyLQn2GeeVovKuphhe6zPUVEpdYi6rd",
    "SVmkfGVorN4oDA5YwyE4rSKHSMhXCVRgFn",
    "SUYrJwpdRs8xVBNtmD3k3E4gzTKB48Pa49",
    "SZHun3QbXVgrJ9jPv6wYHrdXgJVxBteq6x",
    "ScW9kuhJNFJsFC9qJraSnaVYvErfyKMY1E",
    "SgpuS9uqzTXeSBBHG91aEY3JLUvnqQyh1q",
    "SfCBBoV2kPFhJAVWFn3mB6zGpnvyJDUYWi",
    "SfhQpFkXXChjMeyMLQpuopZy4PeZnoGG7c",
    "SeupYXxYaPa3gsvkEdYeEnECMPSC6dWKHY",
    "SZZKtR2rSz4GmZGqGSTDriHgkbCxB6CYF4",
    "SRA9CyuGiXPQSWNQVFYsjDQ5XzrwbKDws8",
    "Sj8BxGS8eto6jdj6pSVgZdrMTsAFqsmF9H",
    "SZ26PxMUvK5oHKcbLYiv7cXyu1ReV6hsso",
    "SSap88UfPE9CZHPwd8ErHd9TaBb1wMxkpa",
    "SY23V4kiADVHq5N4fyfCY2TYcozbmCA4WN",
    "SaQ6Wf5tzAxR1Xc5xtZofrLgR7u7DCE7Tp",
    "Sjat7HGs27ww8sRyrhGDY6GTEbQouUJCXq",
    "SVmFysvNi4eaakykr2NNmwQmV2x235hHgS",
    "SaPMUi9Lwj2gzbH8sJkRd9V92hVFKzVfNU",
    "Sif2esexP8rsnrvghRKfRqwTBQYBkFXwgt",
    "Sc8T9nHfMiE8ey7MxvF43QwHiPcYhu39Le",
    "Sg4TdofZgVPpx7JQVBkpqput8B6as64sQ5",
    "SWcYmJVw5stE7CZbc5U1ULdQQGX8HMRfih",
    "SbZTpUHMhbE2cH3siXdx4a6YaqVjdGvA7j",
    "Sc9dLtGzDVWmPtEBE1socSRTqsLqR8zkLH",
    "SWavi7bW5WedXxgxE3G5CBiJwkmgC4Bwnu",
    "SY3AH68cg2L1BfjJWQF2eULupAfc5dqeD6",
    "SiKPubPeKy8WA1FkmHdsoa639Twp8r6261",
    "SRqho1FVAmF7ML5gU7Z7oR2eVJauKpzHdX",
    "SabF66vzyV1P7kNckC8euefLtJNq5G7xG8",
    "SfbgNruvVs621Y6LLLbV3vtwZSGmBuy3Mg",
    "SUpHrAbJ4ERPNeNaLjVKaVVHr7fMtDTk7M",
    "SQZkkhJgg7Lie2pvfjcGrFfv7xJ1GQbYeC",
    "SgK87u64iMd69xJNPsBBg3egyXpdD8EoCC",
    "ShGC6UqJy4Wt7v5eAgqhGrKLWNEAhVYnQT",
    "SavNVk3PgQoMwgYokgqx35iER6DMfNK26u",
    "SPkw3MPtTwC2GNEXZQtsXCqAPyLXdkpQBY",
    "SZzAuLzFJBrJ2K8ANpta8QhMJzYSTJ2UYd",
    "SdTBLdVVs1U2YYUbbmQ5WVjZm5CM5xccat",
    "SegBhtaibZZRzBwbQ1GUVZWfVgKhYdkHg6",
    "SVjHDsW3u88z1RsmCrHmdBmrLapxeQTB5Q",
    "SguPHzg4EVXZSBSnv1iFeJAiGK6Gzr3iFo",
    "Sa2BfhpgkuzYWqXQuwYY3Vxtf8QsGHNWQY",
    "SUCmk7QKZkG2bM1XnxeNmHXFNgpumyCkaC",
    "SWaRUqLEECDd18ZNi3SB8SjZemNabR9ESC",
    "SbQqoJCLFipYjuEDVPQuQLeP1i2RZJpKCm",
    "SdfmFN1bZC51yAvQxJoQqUaLZfKiS1CZBx",
    "SPPPK49ULLahmipp1KFULjvbHicoykDyvf",
    "SVu2psScBfX4HDXsDZTspBqc7QSEDtVoG8",
    "Sde8aGeC8vK9qsDekFbaF3yC9fwPaZMs5u",
    "STXWAt89RfcJtcPkqrYDi81BiH2iGLCSxA",
    "SRsmCkwayAtjknrqA4r5nz2xkSyezDcSB4",
    "SQLeuFqBGYJM86fLMLQhrzU4fCK2iiqQsz",
    "SZ8y16pDGB2Uyie9MYTZzvSmuzGgrkmjed",
    "SjZuvzxXRge4i5ygQGzB3gnCXPAstXU6H1",
    "SUK87LK7wkNc2wXmVS557ydrPuSD7wvJJY",
    "SWBEWq4aSVAAEXWV8DmX3CEd1THzAMKyNV",
    "SgqgudtwAVoQmKe7pfnnKAEi4AzLZGTtoc",
    "SjZE1hiirHomdP2aUGUA1szVQBRZ2cH3oG",
    "SREd5zqHgS5x51mfCeVTpBZ9Ci22ZNfQbQ",
    "Si1miUvcXfdXDERmiFVqnsbZPNk9eLcLzx",
    "STDiAtpTtEzV6gVPTzeAiEFSGs5EnMAUs1",
    "SZPEtzMzxA6ZS5nJyzaiVyJurUrhXbncEG",
    "SgoiokKh3hqpAbYyFmPJBQaWbetJmfD3Pf",
    "SV9hyPABdorjHSS84i8bAfoBXLeedc8ax2",
    "SUFNG2WSLsCkJPfC3mcZubRqYpPbCChjoG",
    "SghoXQxhA4JsszsebJYFpWRSaEoa1ZVHVn",
    "SgLyFu2MFkbCKxieAWyJvWFU7TT3kyc5eK",
    "SbPuAtorZbijp4tqHYjdLHBDLrzsmYcwKe",
    "SR4VcSjBNq1UPdLqkPa2Z3sJ6SGSbhDaML",
    "SW1s6TGcixZWcNA2fxMiKSfv6a43ue7Rrx",
    "SebHuv6WGAFSEixDWjdCTjSD5YAMiCrFbA",
    "ShYrfMpGUZyyakVHe2r6idjeL2gJqLZJed",
    "SVW3oQu7yxDdpbF2T7DAJHsfkfMbceegWV",
    "SW8VMN5R4yaDS5uR86uhB8oEriBqh97acy",
    "SbiZYxGkcmrQDv4tuhadehKjW25VRLNqFP",
    "SR6gY1SFuPtNkEvmUeF5iUXGxtGwNoZMGX",
    "SWHgpKcvddG238e5sQzbqFa37uQHuznZtS",
    "SWf5B8RtuVexcaMu8LxkHngCN2JBvakFDz",
    "SQnmCBtkNioSdAy5HxZAPYFQQpfQCdYGPw",
    "SWkGTDpeZkRMDLYQbzJTd657V7EekMmoiq",
    "SUK3oD6Y2eLDjnKJdevpC4REmQRNYVwUu6",
    "SRdxvpzLbxmepGS2TH6nqbjEn821YBUpqa",
    "SRF3xWji8SGb5MMR57wDWCad5kf861YJRH",
    "SiRsNT8BAAvsve96N8qYtJpb1LPgP3siYF",
    "SURhhXtn9yM6iSo1YKZXHhmvxD78FysGPo",
    "SWwDP6wRd9BYBfjTrxYrNNo6FFD9Wd1fuQ",
    "SV8qavMKNERRfnqqK5XwgXQ4g5U2Aee7id",
    "SZHZwmBZjcR3KnnCuEBCHsJLJwYpZRtSEd",
    "Si5nrshFjEB6N6yTzvvZd3NWjNCZGAiwMw",
    "SiDLVWSsrFKHTouFHF11Nd8bdU142TftJY",
    "SV19oQswgiCVHozEMaXSYT7uGb1qA17R5f",
    "SgvLQ1GC5Zj9YLMqBYoPRP2R84h93p728C",
    "STMSgCdc6fUpQJMKTQ4xdQccuMEUK2F23k",
    "SVtdoDTor6LKD7uS36uFva6zRjf2JnZ4y6",
    "ScfinqKqMyTMDsZyuA6oMrVUvc58fKHfaU",
    "STqYEsNzjJaor4Y9X26tNjBKz9NE1r2fNr",
    "SQAkBFkG9tQwmBd9hvhrxgKdee9XSf18Ri",
    "SSJsAWJFJSSrZbkKzBifL6PEZpJNjAtzL4",
    "SbdLgxMnZYC1Lbp2aVM483tPBJv8SooHo6",
    "SUDonXfKr7Y8opB1jgPPz4QN3ivWmTMTjh",
    "Sk7Ew5FqkUJiLvGMXcneicC89x65c197vp",
    "SPs2Cm5SREZssjdGRSniAe4jgdSQeafipw",
    "SdYiHJ46PF3RVxn8UGcktfxLv8wc1FwWS3",
    "SXztZogyniJ2TtoH7KijJjJHBQyqx9pinx",
    "Sf5gpXm2DeXCNH2Aq9n9XbcjEX8YYFrwYj",
    "SPVcLpKSRhxWZ8Z9CBWGekM1xBtrwUwxLQ",
    "Sfcmp2egYJ41gbvy5g7TYm7863Jnh1bZxC",
    "SazEQEGcReXSEne5d5uxZUV1nfy9daGcpU",
    "SPLQhvofRqX5BmBpErARe2ZUrKcnCxLtT7",
    "ScVtWaJhTiLZgUg1A143TaUfFwfGXm5S8T",
    "STbJ98VbL1rrb2LES4ksNN5Jp4Q2ezvp84",
    "SbKFW5rgocnbUGvkfq38cgf8LPKGJrrCxt",
    "STfa4bKjBxizvuGKmaGkEtAd2t1k13obdW",
    "SZcjWSzAXCzU8VYCxpgz9tTAgZoyDR3fDk",
    "STDXKzZ2ejWu8qCjqmH7Ckc6hVhHz3VNoo",
    "ShdXKA2zopB6DXJpxCoKTf3YHdiNCht5mM",
    "Sgdvyd7G6Km7TeYHKxZ8F2f3nDtpWchnXz",
    "SPZHoDDKovRNtAFiv99F6zWgWQMXVmuEDS",
    "SUpL68uQ3yQDfxacMbnMW87mYRb5M8PYbR",
    "SSXtsAQxVm8YYnT6G6KeY8niGxEhHqpm7v",
    "Sfp9Rwsgogmw63YS6wGGiTNv46yTaDFSiC",
    "SbkZj2PsE9aia8aoDzh2gtBh5XxgiaB2fS",
    "SdixY3MeGzcLs4FJmapfYU1KWtgKyFPjCS",
    "SRCKRqs4dQbo5XtSG6w7HzJYfZWot3skqS",
    "SkDUgGX5yNYs67JSPsK988qGAFC581tcnc"
    };
    for (auto item : pba)
        mapFilterAddress.emplace(item, 1675976400);
}

void CTxFilterManager::BuildTxFilter()
{
    LOCK(cs_main);

    InitTxFilter();
    std::string Address;
    CTxDestination Dest;

    CBlock referenceBlock;
    uint64_t sporkBlockValue = (sporkManager.GetSporkValue(SPORK_116_TX_FILTERING_ENFORCEMENT) >> 32) & 0xffffffff; // 32-bit block number

    txFilterTarget = sporkBlockValue; // set filter targed on spork recived
    if (txFilterTarget == 0) {
        // no target block, return
        txFilterState = true;
        return;
    }

    CBlockIndex *referenceIndex = chainActive[sporkBlockValue];
    if (referenceIndex != NULL) {
        if (!ReadBlockFromDisk(referenceBlock, referenceIndex))
            return;
        int sporkMask = sporkManager.GetSporkValue(SPORK_116_TX_FILTERING_ENFORCEMENT) & 0xffffffff; // 32-bit tx mask
        int nAddressCount = 0;
        // Find the addresses that we want filtered
        for (unsigned int i = 0; i < referenceBlock.vtx.size(); i++) {
            // The mask can support up to 32 transaction indexes (as it is 32-bit)
            if (((sporkMask >> i) & 0x1) != 0) {
                for (unsigned int j = 0; j < referenceBlock.vtx[i].vout.size(); j++) {
                    if (referenceBlock.vtx[i].vout[j].nValue > 0) {
                        ExtractDestination(referenceBlock.vtx[i].vout[j].scriptPubKey, Dest);
                        Address = EncodeDestination(Dest);
                        auto it  = mapFilterAddress.emplace(Address, referenceBlock.GetBlockTime());
                        nAddressCount++;
                        if (it.second)
                            LogPrintf("BuildTxFilter(): Add Tx filter address %d in reference block %ld, %s\n",
                                          nAddressCount, sporkBlockValue, Address);
                    }
                }
            }
        }
        // filter initialization completed
        txFilterState = true;
        LogPrintf("%s: Tx filter initialized, %d addresses\n", __func__, nAddressCount);
    }
}
