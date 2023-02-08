// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2015 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2020 The PIVX developers
// Copyright (c) 2021-2022 The DECENOMY Core Developers
// Copyright (c) 2022-2023 The SafeDeal Core Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chainparams.h"
#include "chainparamsseeds.h"
#include "consensus/merkle.h"
#include "util.h"
#include "utilstrencodings.h"

#include <boost/assign/list_of.hpp>

#include <assert.h>

#define DISABLED 0x7FFFFFFE;

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.nVersion = nVersion;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of the genesis coinbase cannot
 * be spent as it did not originally exist in the database.
 *
 * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
 *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
 *   vMerkleTree: e0028e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "SFD - this is cryptocurrency and the main fuel of the P2P platform";
    const CScript genesisOutputScript = CScript() << ParseHex("04c10e83b2703ccf322f7dba62dd5855ac4c10bd015814ce121ba3260b2573b8810c02c0582aed05b4deb9c4b77b26d92428c61256cd44774babea0c073b2ed0c9") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("000005ceebc6a879d05fba128637a36123caf596dca64233c33012e5c3f20c28")) // Genesis
    (370, uint256("0000001dcd4d1cbf2e6aef5860957c71f2e37a10d5d9992d325013e49a690e28"))
    (371, uint256("ed8d0e77ccb32c72ec2a650d1dad70cd084f0c06092d9bc2bc692dd93109ebf7"))
    (1000, uint256("edc909c73f291fa1298197fff7d6bc8bd09f288c735ac85b220fa953530f453e"))
    (10000, uint256("ac25bc6d667e52f9694829b086d280a8d80c3596c19c5ca5043e0e50e36aa748"))
    (50000, uint256("2f50f49216c446169b33f5c35b250a49076e9b429df4082f409b28347dadaccf"))
    (150000, uint256("29381de57c72b9423493f17899efc08397826821b2d2fee47414b0400f377c23"))
    (500000, uint256("8e43d0345693e9131b029b297dd70771a1ecd40c1a74389d7885a5d723f030b6"))
    (600000, uint256("2606c99de74a9788d4f3786b0a312ed3f85bd5284fb7c8156f72449475a3fd11"))
    (700000, uint256("98787da099c78c2703c79966da9decfcb368d6a437b7c4107cf2009532db7fa9"))
    (800000, uint256("02588b40af266ec93afdae6667d9b5927468bde28f065f205f0635017e50cb8f"))
    (850000, uint256("bbd3b9180291db6c15932907d44e754ac6456019864ad5f795edce4e854a8742"))
    (870000, uint256("06c628d044aeb7e4e59a72a9eec5679f0ec0c961eff10c779a0b9bbdc51e57b4"))
    (875000, uint256("6fadf63387bdc2d3bb6cb7a2aeccdfe2cb88d473a0b9bd1a403de3c9b4cfa8e1"))
    (875958, uint256("1cedace1a0893454327518e5b85641af10c38fc38f0b2f3ca2e7dc357a946c37"))
    (919300, uint256("d6fb292705fcc93b64ca665cde68168d8e84908343b9c01fa25960bd83e87edf"))
    (958610, uint256("229337b042409072c4654e54dd25eb740a73c33e8efd45976540dedc336cdb9a"))
    (1139812, uint256("b120723e645243ca6bc46d0904c1045aa19749bcf1306318a9c14eea930f4d03"))
    (1162329, uint256("5bd4a7843e661d18c37c6f3441a0558dad2defd214635fb009dc090946b4634e"))
    (1186091, uint256("5cc9e9bd0cfd5cbe1dd986d8c7811d8b8a6f56563a167f0b3d3d72da3c8b6da6"))
    (1200000, uint256("f918847581dbcad466b241382f81da2d84f9102f86a52548ab1911fc1bf75ba1"))
    (1245264, uint256("ee28734b57cfeee5c123a599764caad2a2de469e906aaf41ba78a1a05582f0d2"))
; 

static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1675845960, // * UNIX timestamp of last checkpoint block
    2634393,          // * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the UpdateTip debug.log lines)
    2880        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of
    (0, uint256S("0x0"));

static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1740710,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256S("0x0"));

static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1454124731,
    0,
    100};

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";

        genesis = CreateGenesisBlock(1598176334, 2497489, 0x1e0ffff0, 1, 0 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000005ceebc6a879d05fba128637a36123caf596dca64233c33012e5c3f20c28"));
        assert(genesis.hashMerkleRoot == uint256S("0x6217b988a9ffbd505d5302da6cfba90d0c5ef4b82d4c7b8926a850ea2294df9c"));

        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.powLimit   = ~UINT256_ZERO >> 14;   
        consensus.posLimitV1 = ~UINT256_ZERO >> 24;
        consensus.posLimitV2 = ~UINT256_ZERO >> 20;
        consensus.nCoinbaseMaturity = 60;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMaxMoneyOut = 10000000 * COIN;
        consensus.nPoolMaxTransactions = 3;
        consensus.nStakeMinAge = 60 * 120; // 2h
        consensus.nStakeMinDepth = 200;
        consensus.nStakeMinDepthV2 = 600;
        consensus.nTargetTimespan = 40 * 60;
        consensus.nTargetTimespanV2 = 30 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;

        // spork keys
        consensus.strSporkPubKey = "04D64D2442D72B7FF2705F4CC474A66527102219ED51AB7764D8791D30F5F962AFC6F1AF763AD1008625434FD7597B23533AE803C4BDA497C86941B5DFAD1D3AF9";
        consensus.strSporkPubKeyOld = "";
        consensus.nTime_EnforceNewSporkKey = 0;
        consensus.nTime_RejectOldSporkKey = 0;

        // burn addresses
        consensus.mBurnAddresses = {
        {"SYmgtbrmouVwnHqtjxTjqpmCKVKDz7eTp2", 1186120 },
        {"SfsfbNsC2B6gA1hZ2KPedteSh3MucLm4yX", 1186120 },
        {"SeWmpsyXh9mcXe1zxZLyWbABqbSiTjnqts", 1186120 },
        {"SfKwYZ6Nz5iHrZpcoFqfx39i7LDkJeHJUj", 1186120 },
        {"ShbSPKotKsv9PBUSNSM2iwGZaioaJxTS8p", 1186120 },
        {"SPu54N65LdHAXwvuQpBtw38CznRw9sDhc8", 1186120 },
        {"SRMkrKXQ1FgRHwoBoW7cDSh6C5nJuwFr7R", 1186120 },
        {"Sd8AY8AM5ArmbQmEMf3N9C57V8f1kkAtyg", 1186120 },
        {"SUejJcFHgpeimYarM3y5KgUe5arPJDEE5n", 1186120 },
        {"SdK8so5UUY7NFyjExwD1Z5sjupDq5JsCA5", 1186120 },
        {"SfWHCDxWb6B28NaAEAiG2qXsTXFEUxjher", 1186120 },
        {"SbqKSqV9jz3KsMSk3knyVRSpqqCwUwNmmU", 1186120 },
        {"ShzTqT2nAg2X1PkYwLcz1GnHJkg7NX55Fh", 1186120 },
        {"SiK3vGAQszYTbXvKNoo6M3YMiMtjESR6cu", 1186120 },
        {"SbihfB6gKtMgzBVwJdbwfPsc5eN6k1qnjE", 1186120 },
        {"SjL4SfGXxtwvfTseKZPKhM1nyfvrEmT8gC", 1186120 },
        {"Sayok7DbWGUzF4qs2jdxj3TVSH5WtzDfp5", 1186120 },
        {"Sap5cidKzpyw49iiPKRb5QHEHdxJWKuXLj", 1186120 },
        {"SR4LocL8MMwrjnHDLggq3RQ1i8gStjnbWv", 1186120 },
        {"SWwdb6WtPnv96BhNXXoWo3HTyMuw5NseVa", 1186120 },
        {"SUrj28tAck7Jtpec1g4jwVvpvh6tZAGfhE", 1186120 },
        {"SRwhk3HyezHd5zgQSY7jK4wuiB5dyKwB5S", 1186120 },
        {"SVYEXBv9P9SzBMXjWq1ShFL6FGsNTMQquY", 1186120 },
        {"SbGiX7UTTS9QxzFDXz1byKZ7pzayCWTiRY", 1186120 },
        {"SRCRgtFr7vSiD8yGHm7d6PC4So6RqN1Hwe", 1186120 },
        {"SV5cMesfxqUQumnWhHfatcQLJtqEWpZFi6", 1186120 },
        {"SZjn97dmbg7n2TfvuoNWrBrgErzFNA8RVx", 1186120 },
        {"ScNBp5nuiFHHRUdeHJzqz3DVxASTLtjgP8", 1186120 },
        {"SR2w4Dx33VHqemZQgY4eGu7dhMt6GkNxBH", 1186120 },
        {"Sdc8qaSXtFKmNvNv4uG1D5UW9ZbnNmZVRZ", 1186120 },
        {"ScjgMpfcnSSgMxJLtMqbKZrCHmnSdLjtdJ", 1186120 },
        {"SZLZXaK728hJXyqVb3J8bkzC7EG8AUgz3s", 1186120 },
        {"SfWwiNSDDEwVkxfzCE77LsVVjaLATCjUto", 1186120 },
        {"SYK6v4w6nhkFPxM9ek5UGdzXgrxRWcaPQt", 1186120 },
        {"SZmKoJEs1xKu7sx3PHj6dFsaUBx7Et1HFo", 1186120 },
        {"SSmYfBHUqdHQX3wwf3pobTniWJQhUWzRLD", 1186120 },
        {"SNsysjgVuUYa8dDqrio4XdcLT38t2EgQjZ", 1186120 },
        {"SYmbYjooC6Mgs7TzJiyYqMBGYjGw56jghf", 1186120 },
        {"ShvxC7WQ9mL2pmJFLUWWo8ynioADFjp9CV", 1186120 },
        {"Sibm18U3CgNXeo9CHpvNja8ZLGkL4RhSHm", 1186120 },
        {"SYo21rW2PC9VtSNV8zDToMkkjS43zjTEie", 1186120 },
        {"SbLBbtYr2QFT5o6dZ6SYhi1FnGV4f4cTr8", 1186120 },
        {"SWjrT8CyUMnqjkqd8ehtCqSk6ZKmjDuGVZ", 1186120 },
        {"Sd6KauWXUYxsJxgtzk2iMcdopy8584TTdU", 1186120 },
        {"SVmrj3gMA54qgM8MEnxfrqSXgqkEPGRtkG", 1186120 },
        {"SdL1R2aLNT9hyXBFDDTP6LtTXFXUCYJs5a", 1186120 },
        {"SNuMRJYUSzE2KY1qPyN6Kp8AMMxz4uV1q6", 1186120 },
        {"SbbfzeKsv542yfWZ62X7VvsgAXnDPApZPQ", 1186120 },
        {"SYp3Boyc7BSZ89k2TAWm7PwvVEbo24ekTV", 1186120 },
        {"SStCurFi6mGHj1mKTjkM8abTop9hZL76SS", 1186120 },
        {"SSrmqqJQhnjh8Lgbabsjrdw8Y8ok6Z4v4v", 1186120 },
        {"SUUksTQWzmUssYsQdAHmxqPhReMnwjbddT", 1186120 },
        {"SjG92C26w99a8RcQQfLMWgvcVvhBNRwDjo", 1186120 },
        {"SX5r4jXuosLhRRSwT3R3wZ7FwGcFyb6t5u", 1186120 },
        {"SRzfH4efWjM85kHJ3ubbjGitQ3gAxfZ454", 1186120 },
        {"SZhu8wefPXbStydgdNdR7sQAtFAuVBYVyb", 1186120 },
        {"SR6HcGhp2XceGP9hYMrV6gGWgSG6shXkr1", 1186120 },
        {"SekwgyAgsoBQK8C9ewde2U7zHD5pej5BwF", 1186120 },
        {"SNiLTQPtN3C9f4uvQaSzzrbb4WVRhbXfE5", 1186120 },
        {"SbCbbBeZbBzKYQKc6dLUoX1mrvveKSFHJJ", 1186120 },
        {"SNgAKY7JTwj6rVFH39c3irkpCSKy8CmK1q", 1186120 },
        {"STiqwmsfqBuF5MEok62ijtV6LLcLs2QuXG", 1186120 },
        {"Sb6b2oMVWE4wMokaAjfdbzvnrLCjMyfZEF", 1186120 },
        {"SdXaBXAMcoJk5U4dRs5ADpwKo7cHtdqtSg", 1186120 },
        {"SjtMGxPxBCiyTNMyY8BBEhhj6sbkLjMWdZ", 1186120 },
        {"SVhPWmqB3T9YpetLk6pRD6reuRZYdhWZyr", 1186120 },
        {"SQof2ZDZsy83ZEDbqtdr5XqEVxo7gpyTFU", 1186120 },
        {"SZt1c2MFB2Yv9reCdfsdZt8tXZ3BC754DJ", 1186120 },
        {"SdbDUeEUpHcH1KHN2FUfMNPwarSHJkX8qM", 1186120 },
        {"SX3bk5hhLip5z8qy1CDCzhHBLnC2dPVEC8", 1186120 },
        {"SeXbTCu1ZTbgkvMhXMiyr34NeN42ZLqcRE", 1186120 },
        {"SRBivFv85oDxAMozujR5pn2f1vG394jqqA", 1186120 },
        {"SSKkvY331K3kodVKDjVveCSHYyJGnb525y", 1186120 },
        {"SYefrTMY73jroGq4sC2tQc3LXRL1KoLufM", 1186120 },
        {"Sd7Z8Jx2Svho2xjRkXSj4dJ8fpXqDfNAMG", 1186120 },
        {"SiWKtZksdtN9RyfPU4zun3QzeWNam6LsLm", 1186120 },
        {"STxx5cWMNKyg79RZMuwLXXDPWiMemRewGv", 1186120 },
        {"SNPtxr3NoLnSm9uzahYoLssLEm4CUJkLti", 1186120 },
        {"SQFsX2wqGrxbytoB2mKGPW19J7Zu5ogqeh", 1186120 },
        {"SQ8sETxvLitsGuV6teTVkGLEks6mSm9V5h", 1186120 },
        {"SNpyAThp38LCHS2ZkPozYyLAoyUHDoTVMe", 1186120 },
        {"SStp64XE1dcAZdk484xoEfTTm1zKquxmrX", 1186120 },
        {"Sj2JwKbPXdCJ9FczQgc2pYZRb3ftZgrDR1", 1186120 },
        {"STMTE1fsBfdtMGh5RHDfvbQy3A9ejQSuBF", 1186120 },
        {"SVz8PJUvwMDXsxhwAnqjyZhKXZsWVx2Z8j", 1186120 },
        {"SVnBruqc9JQeqEZExPyr5zQRFYprFoRWoM", 1186120 },
        {"SZ1SPeDLGdFF9vtn5s2mWnLLrL17gQrfSx", 1186120 },
        {"SjeEFNf4aWpeK7QwRoZkupdJrpYe7Xd5pW", 1186120 },
        {"SagpojwsVo2cXorttEBiu1yG1LtLbQ5DEp", 1186120 },
        {"ST4JLd3T3jMt4qC3kGQuAM9MZF8RSTvSMK", 1186120 },
        {"Sia6xKzoxFjcgSkNUNkUwu7tVWixJVwpfS", 1186120 },
        {"Scqr8Xw5GRWf6PkMVrTrGy9F6ruX7B7SVW", 1186120 },
        {"SfopRescWkWExmTNQxs9sM8F7qpM22A7u2", 1186120 },
        {"SbJVPNTtXiBhxWrqfrqHUrySDheptoY6gE", 1186120 },
        {"SgYW5esZieFqdK3w2qMeGVM7K6SQ9bSS3C", 1186120 },
        {"SUcc8xWiNBKsddnKEyrqNY9hucQbGJXmM8", 1186120 },
        {"SPnbkmxSR8XhgfwqdE5JWnxQ6XDwZBrrQi", 1186120 },
        {"SW8RmHBqgLN43poP6WCXTHJrwrcbJoimVw", 1186120 },
        {"SW7pJJiZW2jMcGQg9bySHZW6mKYuKKAZds", 1186120 },
        {"SVbGSqZgGUFuUrXPXwWARqVYxQrUSzBexZ", 1186120 },
        {"SU5zWetuqESFkjHH9dvbcYF46xM6kUtezT", 1186120 },
        {"SPgB57X9A1QE1e5hDPRu2xj4CpexcWfqLk", 1186120 },
        {"SNnqBtHCh3g7ydEiYhaKfMMa6HuiVTmVnh", 1186120 },
        {"STRdpGFpoNaTjyk5Rs1zJ8oqcvousiC4dt", 1186120 },
        {"Se1gqvopeec1mTvvsAqZHXXVYP9tHN5dLu", 1186120 },
        {"SZ7Ven36Ra8j8NQNRejYGYRrbMhD2vVWoK", 1186120 },
        {"Sj7b5S88e14C69dxauh2dKNE3c9ayvXU8R", 1186120 },
        {"SUfbTnbRXQ9XLLP4ncUFf9agtXGqawi9MD", 1186120 },
        {"SUsnnQ2Ts6PMMVoAEo6cM7LVzzahicDRgv", 1186120 },
        {"SamxPV2f3LFMBS5WP787Mm7AMj3yGfH2KH", 1186120 },
        {"SbLou1aPfkPTuwSNJNXtES7zkugRHno1Ed", 1186120 },
        {"SVgn5SXJrZnwh112ejfYjUmftAjrG98Xox", 1186120 },
        {"Sh5oWUXRMWvoXRei7nfLsAdf26euKJ5U37", 1186120 },
        {"Sh8NXvtD6FvrpqRW1ZqqjGX2D1GU11wauC", 1186120 },
        {"SRkBH1zx574T6mFwXa3DMvpEuY2DwbduEh", 1186120 },
        {"SdTVpEg899dv9NVTEwdERhKUjT5fW4NQQP", 1186120 },
        {"ScDezao5jM1T5d9Fm9HytioTw9o8K8jZxv", 1186120 },
        {"SThy87QWGnyrZnFXv3fCu56RqVF2feb2Ti", 1186120 },
        {"SVPFu6qbZSC35aD8BSGex5Bk2hmqRRFwp2", 1186120 },
        {"Sg13osNK9QSj4Gi2DqAMmks6DNRFdMUwAM", 1186120 },
        {"Sd25fracv2Vt1YPzZgXR2WGVcHUXAPv5Wk", 1186120 },
        {"SYaiB74fgF8RQc4HaEYu3FHpvbiLf7r7Uk", 1186120 },
        {"SeLXNKZBXeDHWzbsmuGzQHjM64bhTuCJks", 1186120 },
        {"SPfHeXeYHPn49FkCkXNHRrdaSFMTSsn77L", 1186120 },
        {"SPQzRV8mF7Tdo3ziPRQhhrwS77ggP7bHKM", 1186120 },
        {"SQQmSSNToTtTgWUH4ckzNpSa6kbmmxooty", 1186120 },
        {"SWVJvXjTz18eMEDEPSn7tCSa4TGMJsbJGd", 1186120 },
        {"Scd5pH5PPBcwxmGWeacXH6kvuzBkyQqCSK", 1186120 },
        {"SSZbjrdTLZ7soEMj6g8vGVTrcfs5KMwwor", 1186120 },
        {"SQwYA578FwgxbM5mcEKA6XrYrpgwz5tZeb", 1186120 },
        {"SZuN4wAm3a5TxgzvyMDymMbEofemxriFty", 1186120 },
        {"SUXG3uvGvX3xYF41wuAeEr2XetQUfUvkzJ", 1186120 },
        {"SZb4XMbTuSA7DfeSpGM43fwNVFWEuNyRnU", 1186120 },
        {"Sdso53D2ZWPGGTwHTAkwzuBg9HpvJuqzPq", 1186120 },
        {"SYH3omzmHhVvhvbjC7rvF3mGxPeeRCP5He", 1186120 },
        {"SfXf7ZgZoySiEZYmKcUBukk3eLaac8L5Li", 1186120 },
        {"SjKzzTZcNDLZZ4UswY98Buu5WvvWd4K8U2", 1186120 },
        {"SazNivwWJDZp3RXW24nc6Tzkj43AaVhLHh", 1186120 },
        {"SQoZnYmgKjkcmSdFxmAEfmPhcRjKDgdhmH", 1186120 },
        {"SQg3BhCVNjSC5rzPca33yy4fJsMFUALoeu", 1186120 },
        {"SZBRwrdcitF1ebLzTUxwSvpwUjxLDTSG5Q", 1186120 },
        {"SiX82QVYtq7RPhpPt2SvPRuuMnW9t5XKF7", 1186120 },
        {"SUUaNrLuqScsJrokRbH3ymHAv29A7fGE5w", 1186120 },
        {"SjhsMdxJuG4ZzhXBttXtrMA9FG1KdMg2n9", 1186120 },
        {"STm42JTKKZ6dToZ7hRVXXtLmMowVSSvY1w", 1186120 },
        {"SXq21Cpj4kYBoSK6vwu4H4qvDkmTZv3Rid", 1186120 },
        {"SgZLtiVR6VdsWqk2AYHk6dr2nEogLb2hW8", 1186120 },
        {"SYWtpyS21qopLLe5zhPS68unSUCqmsqhzg", 1186120 },
        {"SZFPwzKmigy4NtM6riAXxNKd1jU7bgkjv7", 1186120 },
        {"STTp7H9JaBWKnTH6LqJHhwQL4ar4R3c3BS", 1186120 },
        {"SiDZvGSMc3Dzinzv1acG4GXVogRqcYEJJR", 1186120 },
        {"SeM7SUsztSb2qFTB6Pv8hKXpLLo66dBgN3", 1186120 },
        {"SPTRPty8Z4MM4kqUvr3Y4Vn7N1HzUY3Ryi", 1186120 },
        {"Sf7AsrZDkJ4mW6DwksQ4oGLFtSbYdVc78r", 1186120 },
        {"SNtmtcQhvANXauNSVPMRz54wX2dc44c2rT", 1186120 },
        {"ShD7xiKnFSyrtGVPdgtzKM5iQpR8QDRXZb", 1186120 },
        {"Shsc2EU9EYgKQGrHMvjgU1XYz7KBcMP9Mm", 1186120 },
        {"SY4QzKPdqzZYenuYcyZZBkYB1rCmjaWQmc", 1186120 },
        {"SQ2bcmpqFYMb75asSVcTwctytVyR8yRg9q", 1186120 },
        {"SkdavmDDPkNGxArouq1LQtJGbijJauMd16", 1186120 },
        {"SXUmVnEftkvctA8KJPDkqivKYyJrrWMWQH", 1186120 },
        {"SiAZEe7oGFiw31cgyDmhyjtH8fk1gEHfRs", 1186120 },
        {"SaXKtoF3v4NFd2yp4Z8fH3vfAwMh9pa2F4", 1186120 },
        {"SX1ETunQEnhYzdCtDHquMfC951mpWeayS5", 1186120 },
        {"Sd4sd9aGA6MNptcJ37YuJQzy3ULvDoZ2gp", 1186120 },
        {"SjCjrj3uziEg3udEo3H9itV1CZUSjRb7yU", 1186120 },
        {"SiKKn2uRmNK68DRAVmc82mvgMMhoH5JGWA", 1186120 },
        {"ShpAApe9TDRRHc4XxC7ZfcTAfbBtNzjzS1", 1186120 },
        {"SR2DmwjNRvTkpDEibNDyzNVBsvLmGrmxEe", 1186120 },
        {"SY5P6YjyBmVVtHsEzayP3JMoq2f3Cjh6nf", 1186120 },
        {"STHri33eu4qUdD3JRgk55SUEAMzoewdKY2", 1186120 },
        {"SScj8rZg8FrQhUojeCNMCLQHDnSbs1JdUv", 1186120 },
        {"SgbS9yknc5DVAqqFWZAN3KVAjSwYQiypYq", 1186120 },
        {"SYG9H6THdQDZL7J83pFEjkpHNM73MazRK1", 1186120 },
        {"SaJwx8RPbd77BCjDc3p9YFPyvbteuwFeUd", 1186120 },
        {"Sb1wpHVTmk9hqDLzAZegXwFNsfhA6zHDpK", 1186120 },
        {"SMSCNu3m15iSrZmceNeCu6Fd31xcX38jK4", 1186120 },
        {"SRRncy4duMVPASpocscAo4WMQWBzhEpBFc", 1186120 },
        {"SanaL4xHPLNU5QEmS85Wjw3FV2Y2h6rAd1", 1186120 },
        {"SgLKzxQEfyk8QkFrs23sLg6CkZafGqubvx", 1186120 },
        {"Si2jA4HYrKExyubqexEwJsmRiZFXSqpKP4", 1186120 },
        {"STsMu96Q11uoBJPpSBZT4wgrkQZgcE3jSk", 1186120 },
        {"Sj3nhDGhgoJ4AgQnb3ncpPzSotA5gBcVsV", 1186120 },
        {"SUeoGC6nixZWfWGFJfNqsbPSDqGtCZ31aH", 1186120 },
        {"SZan5fXkWbCYscasorAfD4utwtvzdWhE83", 1186120 },
        {"STk3hsoboo8Kxm41rGchv8StVBUe4B2oEi", 1186120 },
        {"SMUvjj5dPbFwqrvZBuYbuQ4r9gyDsLuRiT", 1186120 },
        {"SeueRdgH33rTuHe42e5V48AmiYpc29CPFj", 1186120 },
        {"SZKgVGkRjarcoeWny9Ms4pAsrjmsxe3Y9U", 1186120 },
        {"SQKmNoVddSRECZMYnQ5nqoj3DQGtwWKiWX", 1186120 },
        {"SfzuxgQXdFL33WiVps19dHt8E5HudYDiWb", 1186120 },
        {"SfHA1JvAL93aA46sMHfcJxTfSYnu1sgBak", 1186120 },
        {"SP2gZ44xoRDsqZEqNJuCwN66Mjev9rEY19", 1186120 },
        {"SbZqAPb5GnddjY8ffdcYzt6LX6vbpjivLH", 1186120 },
        {"See4h8L7MzN6u4aP2NuCMnixmxeXWohXen", 1186120 },
        {"SZbJ7ufzALJi787D9UCvXwU5pcaypxazyi", 1186120 },
        {"SjtU1A8K1VmzSBp9mjLNrLi8EhxGZWRgcL", 1186120 },
        {"SetYy2vqnAYcaPy367jT3jfDxeqh4UfMnR", 1186120 },
        {"SgGtWAmY5uYFQSLEPj4CJr18BvCGUqenfU", 1186120 },
        {"SXts3wwYgqUcEh6X4UKXtGFQE3L3hVD4Do", 1186120 },
        {"SjMbus7CdGb1JyRSWqJnrAXPD6oZgm4833", 1186120 },
        {"SgJBeFS2XhznSN8LM8mb1jrGkckCggVqcS", 1186120 },
        {"SewxEe2N43bh2aaRnPvYxbW2utRXTszN6f", 1186120 },
        {"SbLXPBPRxK7XPfcRVhpZGHwwco9Yam2GtH", 1186120 },
        {"SUBk58mmQRREVLvL6b2fcBP9m5NTmKvatB", 1186120 },
        {"SbBBgER4MkYASP1vJ9mh4QqsMdjS1rSsvK", 1186120 },
        {"SSbFr4UtDgPSfmYjipAHiQp7EnCt4TdKz8", 1186120 },
        {"SS2BS28WikRBzdYPjpyMRkps66BrAZJ9sj", 1186120 },
        {"SibvixmtXStUPSoHybZ6B3f1CxPNLabris", 1186120 },
        {"SbJE6WDhgz6wS7zbtrmn2vGeoxGGCknZjG", 1186120 },
        {"SjAfqtXckZN9vqr6KhuvjUyo1FxfSBEr4R", 1186120 },
        {"SbL7jGhQAf6xpMmzvuycvZ6ucH7chZpr9c", 1186120 },
        {"SaXhGQhdpTw7zRrP8FdgZEAoybBjqiK6Am", 1186120 },
        {"SSA1sKWLHszpjGwg1Pxu2jB7MWwT1A3m2i", 1186120 },
        {"SgvUcnx4RqhydTXAEBRyH4mjRfvo6oBF49", 1186120 },
        {"SdA3q9HQVjMkqcb5Pw1DEBEhmvScZbzpq7", 1186120 },
        {"ST7Zk3CrqP1Knnsp4aDFAWHFU3rgmKAH3r", 1186120 },
        {"SheW6jsMNm3CcXAAqKtqbrLYwpuMJLQGZT", 1186120 },
        {"SNdQzr8heAzrZZ1G3VHJUzfKzvRwP92BAt", 1186120 },
        {"SZjj6ayhvkQPg4BKmBFB7FiwghQFV2J2QL", 1186120 },
        {"Sk6A2swzWaRoNoTfnJerCBYUtRdy37N7h5", 1186120 },
        {"SggUghzzFWEzXm385qHvPaKAVsiudexnGU", 1186120 },
        {"SS38NJ6bAVZox9YG4z7HYXkFiCtARWSWcP", 1186120 },
        {"SYwEmgdTQbov6wWsgb4mjBAZ5hA3VNGKZ5", 1186120 },
        {"SSwpJCP8BzSoVCSFxz4bq4CBBfegnu35NK", 1186120 },
        {"SUjNiJyrCmNSay9vRprE3v77BpcKwyzcBq", 1186120 },
        {"SapWWRGD8dPfGVKo4Kr9zTwuZP9gLoHHWV", 1186120 },
        {"SfYqp5xSdt4qYNXaV34YBpG17AqryEnxzQ", 1186120 },
        {"SXrUQtBdTZrQeDsvPtmMaeEP25Yn9hG22a", 1186120 },
        {"SgvasLLGAmrrJhrfAVSTwNDiHRU4Fa9Nka", 1186120 },
        {"SfQhDHJZr3ScCuyJEtydZZosnrjHp7U7Fj", 1186120 },
        {"SMfTsJTgo3KyACR6rSAnWEm9DztbowavZH", 1186120 },
        {"SSnGsgu3voQUf5r7W4X9DCev8uhoU1KS5c", 1186120 },
        {"ShETyC3U1j8cELyVAcBcGWJcdz4BEmC4og", 1186120 },
        {"Sc1H22FkkzJ9TtbsZgwbCBa5mDHHkFdZMu", 1186120 },
        {"SV5oPVpeZbP27kZmdNUdqoVCgYMb2mnr7g", 1186120 },
        {"SVPZdaM4PmxLR8g85rsotnXRErxD8bhkEc", 1186120 },
        {"SXV11KNLfpQimT4HJHt8oh3TS4DyP8iHt5", 1186120 },
        {"SVU4HRqfuem3qD38cLgq85FTAChWTYNvdp", 1186120 },
        {"SZTivXXMXgG2Yv6pHjJB2VzBeNFA7sLgfF", 1186120 },
        {"SfgDNSPNbV8BLM3H6RSRGUzgMarcCAaYL6", 1186120 },
        {"SUFrwypMRPC5N65Xs3RWDZTHBVCrWmPfdJ", 1186120 },
        {"SRFGj4HoGbWxS9uXqtTTr4cB6K3nJD9M5b", 1186120 },
        {"SXEo7tp4vVZj8zLj9ouBZtntG9m8GQJGbn", 1186120 },
        {"SWfjLYZVfGsAxibgwHhzLtkgQCDXJyAgRv", 1186120 },
        {"SWm1r1qdFjwreyJ48T9oea7C1pmUM28zCT", 1186120 },
        {"SWy9Hom2jrjyNHKt7EQ6skjbidRfMvngsy", 1186120 },
        {"Sghz24sZjB2uHpviCAxgkvGM6myhQh4LsB", 1186120 },
        {"SgQcqK6EJEAnnLsdLPttjwgCJ72QztjGce", 1186120 },
        {"Se58kxeZdZzShxH7FjyNEWhY1jNqWk4qvY", 1186120 },
        {"SYBmAM6brWQotkEh18uNf4c1ks3Uhy7t4t", 1186120 },
        {"SeBWS7mDzBPPMtwU36VexPRVfUay1pLXPr", 1186120 },
        {"Sjup2Y1ibX9kJYWPJUQB4CbfDE6DotTun6", 1186120 },
        {"SducyJTDzqzyUdSHdeb3NsLNKjUjk1vi9M", 1186120 },
        {"SWibSEY2GjWqmQEZcYcpKgKXCLksxgrpP9", 1186120 },
        {"SWzHwVhpjhPZYtsi7HCq5ECEh1316tTrZN", 1186120 },
        {"SQWfJjPisgqn7Sq93x5StEERB6DaKFuG29", 1186120 },
        {"SjmGK4xh2Y7oxsgEoqJG9ijdvixRQqqFQk", 1186120 },
        {"SPuFUtQwmgPiMexRKxY4BddXJn37d8D888", 1186120 },
        {"SRkQE4x9mN6xKwzMpEaSMY7iXByYDzLDPk", 1186120 },
        {"Sfp3S6xFsZ7VqU3CC8NR2U18wPvfyE7GeU", 1186120 },
        {"SQUBog8c3KJBQVtEBStiqXQQ3TwrfUx9NG", 1186120 },
        {"SSrxHCTQhU5mRfCa9g7j5dgLEnQNnyfb4F", 1186120 },
        {"SZaJsbznp9QhXGiarhopYMpNS14JYveyDk", 1186120 },
        {"SRhP5GCCX7bLvtFDMymySPyrMfjy7y2WjS", 1186120 },
        {"SYVSo31rZw9wZ1TsAi3gwZG6QPeL8FSWLR", 1186120 },
        {"ScgAnimX57p8pNdqUHhsqc7bAkLdoHZnVx", 1186120 },
        {"SeoKJnB8vTGGSWrgH139Sgj4wQqwdMjdzH", 1186120 },
        {"SdE64UrfEeGiSuWt9gdr4ihsUvTfCLYxu5", 1186120 },
        {"SWkV7ecQFSYG9KJWWdRut4ZTusoWBSArV3", 1186120 },
        {"SdgyFruAEmvNQ5679EtfGA96xd21beqLTi", 1186120 },
        {"SMS2Ecjtfb29H735XmQ7hbEpnHHpTquxrL", 1186120 },
        {"SQioVd4KdpMNobC9ebvnEJv4vYEKzErtpE", 1186120 },
        {"Sapj5mK5FqrfHEchFHETL75NpkuKusxspm", 1186120 },
        {"SitRb6TTRqLn4C69ekR1VDzpJCtxRoUTRQ", 1186120 },
        {"SSZ5neaqRz8HqRvLabJE1H2kAsTb7WviA4", 1186120 },
        {"SgzuV4NiDhVtcNXzpk2VJw36vjXNVjBcJp", 1186120 },
        {"SgX2eC9Lu4pHyfuSgJooSfxK6KKNrdXBrG", 1186120 },
        {"Se2hpKztMmz2apezN42wEaEzRZ3CVvZwk4", 1186120 },
        {"SdeYHKfykq2gBe9HiEb2o6AoWVv2zr3kLc", 1186120 },
        {"Sb9AW2vn4jYaJJkB8VYcs6gtyZ5oWV2gsD", 1186120 },
        {"SjMReU7SoKvhQE4nGvLtjPrn71JwVT8tBo", 1186120 },
        {"Siz6WAikYYJi4wnRYxZ9y7v836K2XgqiiF", 1186120 },
        {"SeshAwE3nXryVYTMPziNVbnsFuMsMDoGJ1", 1186120 },
        {"Siu5KLBLfmSag4mEVkdmtxCtjELaAag3su", 1186120 },
        {"SgaR2sUj3ZjAtANEpYLis7LLWiTsd5W6sN", 1186120 },
        {"SgkHiDrYSkAAoBCgQiar7hgf7EuVgmNVkd", 1186120 },
        {"SeD8m3g89LK7Z99zgpaFUv4oDzUVzY8LxK", 1186120 },
        {"SRK7abCVy1JhqaX12FfXQu3TNGQKVAth7V", 1186120 },
        {"Sgf8mTDYbqAuco7nVHfceKnAEsFa4toDFz", 1186120 },
        {"SjyYbXB9RLrtf8THUmhbHutPJUXYjw94jp", 1186120 },
        {"SecZqEgGpvBhZSNXze8tHm2vYVMG1Ayq5F", 1186120 },
        {"SgfZHMpmGH8fTkJ6PQMUsRrMK3jHp4gbgD", 1186120 },
        {"SjF3gZTsfNPnCcsyQJXngdm1o1ragPjvWp", 1186120 },
        {"SduEt2xwwHA33dLmkA1h23RVpZPPCvcZTT", 1186120 },
        {"SNzvKFXz3QtXymjuHBs7JnDFVFsnvQ2e6n", 1186120 },
        {"SfBp4ydFGoy5YkwGdUJ83G9JbJpYNpsXDk", 1186120 },
        {"SbpFwPzfWfherCZGeSeN5EERhBCjsK3skD", 1186120 },
        {"ShhEj1eLvZxetUZt4hShZBFARpkMxn5FGX", 1186120 },
        {"SVtyCvbdXys9UCv62pyHHbqekoYautuVuG", 1186120 },
        {"SPLx1KPnx5a1vH3HkUtEYtAZVHvGTLAE6Z", 1186120 },
        {"SZGcdZ7RkVG2cexzuK2P6dqky2JGYc2YmC", 1186120 },
        {"SaAZW6GhjfQWck4LiSkVznm9Jx1iViPGCj", 1186120 },
        {"SbPPPgfgp7zT7SkLUaZQS1ouhdasn375g6", 1186120 },
        {"SUALdViJMUrs8dGfyPG9WrJwpXWn1ERMSd", 1186120 },
        {"Sj8vUxTMumHiqYCWEEqfaaYEyeXTmPRMPw", 1186120 },
        {"SV2MfnghAe3bsBTv6o4fACMvGj65EXuENz", 1186120 },
        {"Sif4qgo3ZySwFuSuqw3bT7dt2piEeMjzK5", 1186120 },
        {"SjEmwRUEJZ8oXz25PxMPW1uKWqYCGREs7v", 1186120 },
        {"SMVHxfV2K9M5rcFYTshEtFDHg8QZNxxvAz", 1186120 },
        {"SYM3YkcuGSz32HisA3TFUvqp2rRfzMwStg", 1186120 },
        {"SQqmqZY7c9ekSNYKyrS2UBjGk5GpnN7zWc", 1186120 },
        {"Sf7Didamcxf7rmvX5j9rU2aE5BZcVactnh", 1186120 },
        {"SbbNcPJBofmwdg4qGGiycxMz7Qk6iqAsmu", 1186120 },
        {"SjgaZTbTvdkKG3YYDZ7QdsePRcRimRopWn", 1186120 },
        {"SjhvK6vcuNPGx4m87Kecr6DQjh8jqrPk1Y", 1186120 },
        {"Sk6i4vjTbM1KWr5k4B4m3F7XF6w19iobYP", 1186120 },
        {"SU4t2xAy3A3vMCrW1JMwfvANBhLu3a9zEd", 1186120 },
        {"SZnbmK1PsGEvBstXCV68pMSQSRpraPJReA", 1186120 },
        {"SYe567BzFGPr7ysLsL4xKUTpHyE4KwHaM6", 1186120 },
        {"SNSZkEaGGP1eZ6cKCjGY2we6f2RdxuqtjU", 1186120 },
        {"Sehgpn42sJ8EQ82PtxXaXkeHSsrcxAPbvA", 1186120 },
        {"SMM287wrQZey39xZnezsxPHLj1rDbSxdGF", 1186120 },
        {"SW9xVdZ47iUPMik2544Tr7PNvwF5vD7mTb", 1186120 },
        {"SaLVJGWRyUB272BKowFB2VqHY7cStQUd5G", 1186120 },
        {"SZgXvqDqpWC4pzJhybrw8NKvxHQh4JLGm9", 1186120 },
        {"SjqkxLdNjUHYC8UXE3gRDPPLSQ8urHCDJs", 1186120 },
        {"ScYLRAosARuu5xZC7oeCkB5rBVFDuGE6CD", 1186120 },
        {"SfPaEqMuvYfrXzygs4QtQqMvoiuaydnNgE", 1186120 },
        {"SRwXQvKdHMmkujUU78hi67JBdbxcBnx7NG", 1186120 },
        {"SPckABsNCQn5xFRb7xxSUdMciqFhTe4GTV", 1186120 },
        {"SgYvCu7HxZXd1PEcPsLKXrqzKm5e4F8ikc", 1186120 },
        {"SULvarBRvznAPANzJCT7Mpk7DyvFBbNngV", 1186120 },
        {"SPwvRAfd8Pno6LbWe9sBbYDC3qLQPnZKRJ", 1186120 },
        {"SQHy3o7rsFGehWfYPw35LNo8NUSDhQ25sp", 1186120 },
        {"STsgY4TwTZxPF1t241qxpKK1tU9MV1JZKn", 1186120 },
        {"SbmKMG5MPkfJWVFyBKTJDtwMsRG9QV5WTf", 1186120 },
        {"SN2xNXFh6WTZmgbRHJsdWTVCKmW2rf9TR3", 1186120 },
        {"SSN8uBtM6jUgc6Qyfk37K1aYGF8Rw2cvKq", 1186120 },
        {"SkSCbmhpsT7QAPX6w4QkgjU9HNmWFDS7sZ", 1186120 },
        {"SYDujyvpFqzKP1ay1joiPF5myGBmUKR8GQ", 1186120 },
        {"SYRb5iC7VkUh3piDEeUtE17eXuvZXn1n2v", 1186120 },
        {"SexZYhWLbdxdBfP462HbKZ8swLbKG1i84p", 1186120 },
        {"SQkMzV4FLGJahwNWENcUQeZVjskiudb1uG", 1186120 },
        {"SPVievL96hhzqi8w8SBnSZGR5LCddCR5kr", 1186120 },
        {"STb7b8SiKdNtH8JaZcgN6iNicS2Wv6J5XR", 1186120 },
        {"SfKkYB8HvQdFPcyUsbemfQUMKekWQ17s4j", 1186120 },
        {"SkMvqjAHNvMzk6Xn9oJB26bSkzFvzWn74Y", 1186120 },
        {"SZRDSXtSCD2ReSFmMC4JF5et8eb8YCSWqj", 1186120 },
        {"SgWBz7MjY5Jpy6NNwQ1rMDqDrdU4zZW9AW", 1186120 },
        {"Sd1KrKw1tsEx8S8z2eveAC6rHR8cNqooXk", 1186120 },
        {"SZcBwVUVn5xJRKGpNu8gaWdCPKXStvFX6f", 1186120 },
        {"SMtCSW2HicAYD7kn6YUdcsAc9s3Y6EGW8z", 1186120 },
        {"SRSMGPa4S4RcDJ3U8ydrpJ3LSyywVL2ebP", 1186120 },
        {"SWWTqinsrFCBFDwnXJxHJekEmc6cpM15pt", 1186120 },
        {"SWZmhQeX9zA9BRSAAMxtUMbboGqiwj1qUP", 1186120 },
        {"SZYuzNAvft5zjHSGAHqdNTR6gJG6P64J2o", 1186120 },
        {"SUw3JCtUHew4qmzXnc6zVXfmYxtDDmysv8", 1186120 },
        {"Sgc6pTsuVkpA6v4pB4CJRBBbyUSSGfNLer", 1186120 },
        {"SV3U12dig7KVavqv3j4DaX8qDRLwz7RJQC", 1186120 },
        {"SWgPm83QhJZLA8X4rhsGq6xK2cTpwnAF2v", 1186120 },
        {"SQ1S9hCpYE8d8Q9HnNEYBiafhUstA4ZMEr", 1186120 },
        {"Sckp6A4KmoEYxUqqtYArkxsVYMuhgwg9hu", 1186120 },
        {"SYUa1TqN1Mrr7Q7QG9DgGPZE1vLp8Svz5S", 1186120 },
        {"Scnda5YdDUbbM9DV4Z2nsZ8LnhUyE3gqSJ", 1186120 },
        {"SYPyFhxqpbeWy7K1h4moFLwX7XMdq6bjPx", 1186120 },
        {"SYDdLdQDKq1TbtV3AyqekZj3qyFf1Mc1Ce", 1186120 },
        {"SU8GpMc8TkywxSWseM787njPBsDPwmCtg9", 1186120 },
        {"SRwM5JhmcMUmLLkRS67zpV16PbGpXaLaV6", 1186120 },
        {"SdLyTpeyhWAshcnBbhxTY1EVhjj88AjXAw", 1186120 },
        {"SjoR8fP7Gy6QaEh4fbb8dR4tngzuvDnmVq", 1186120 },
        {"SYZbc2jGrkRiXPAxhwxgeAGnjsLCzqxUV5", 1186120 },
        {"SURiHeQCuY2LTKE5sHZtMnTDdPGRE5xE2A", 1186120 },
        {"SUfKG6cCuq1cEhFMLDt7dQ5hJEN7KMgv4G", 1186120 },
        {"SPgrAFsVJf3G496DgwUYeADe1iy5TtrLw6", 1186120 },
        {"SekzTtWYZo28Kxnv5tHxcxeV8suhnM5aC6", 1186120 },
        {"SPvvVNsFrFSFbHa4EGHmDR9G2ruiGtEQDr", 1186120 },
        {"SeUCrZHmaDi2HX3MyKG3oDLbUNNXNpWSz7", 1186120 },
        {"SeL5DjqjDP78PZ9fdVUrDHzhdhqrJ13nBj", 1186120 },
        {"SMSPCCWA5MkRbuxdzGD9x9hGTDDSqUG8Z1", 1186120 },
        {"SbvCg4q3boCg1gkpcQA8q4iyCy8wEiFPzw", 1186120 },
        {"SNAVHLj5hTby4DGypk3v5buqbsouEiqazZ", 1186120 },
        {"SMggVyazBcKGUZ2cFXL6HwbgmbAbG6yDzv", 1186120 },
        {"Si3xW4WxHEox9M58mHpw256jmQCGw4oPfp", 1186120 },
        {"SZugBhr3ionwPMDAnUs3SZj96zqBPz6kXS", 1186120 },
        {"SQgLckeGv3zHHvuZQ6p9VfyigYTvxpgsjy", 1186120 },
        {"SS2UiiMz9Y2BJAPjJrkuYnJebRtGoqsLZY", 1186120 },
        {"SgLgUh146WBjkiukvEThF2oGswmiLYVrSM", 1186120 },
        {"SaozyqNS28KFN5YH2XcF5YWXsjvHHucAHo", 1186120 },
        {"Sa2LJM4iY18YYwMccVnn3VCs8V17p5CX5z", 1186120 },
        {"SVZaHHsmxtGa4dPX3TUL25eWzY5ZwxUt89", 1186120 },
        {"SdQ6bzkcyBm6jXz7nG4AMwd6vWxAChNoEL", 1186120 },
        {"SZGibAmtJiwq9enKZqwyvX6gnshfN2PcUN", 1186120 },
        {"SaZ1mqEDsqrqAAb1TXsT31Mn3sHbWPFxji", 1186120 },
        {"STSK2GDohpmFkpdz77FhtDRQ7XzKxKcAdC", 1186120 },
        {"SPWFNwWQ9RNQ6RVj5grC8j7oA5hmDWS9DP", 1186120 },
        {"SSsJjhJ995FxE66E8sYwU3tzrNfWkPuYHR", 1186120 },
        {"SdmcJxzD1P4fZmwUkZ8kwKBhQpmz4B1Yu5", 1186120 },
        {"SjK4PC614dDChzLZehQS8bKHRadDaHn2Bi", 1186120 },
        {"SeEAj7RePRXmKU4xg2t9sjK7akzwG1tfaw", 1186120 },
        {"ShEkutAkjNJTjPFCyDGjeTak3X49DQ41xr", 1186120 },
        {"SgdaaQ5FrAMV2RgVaj14TJfuy6ESrpEPqc", 1186120 },
        {"STXJHV9Ns59iNqtpzstYL6xRkSNDLY7tGo", 1186120 },
        {"SMfBekbrHapQTvVfhHFppphLGLYeZYuVRA", 1186120 },
        {"STJQ9PfFuTkstvvtxaaCY7f8FjkG2Nskk3", 1186120 },
        {"SdjyRq87cGiWg1a8eHgbEk1E5duMrK1QpW", 1186120 },
        {"ShwHMXxXSLy9NnC8yb4ia3nXPKR7Hhvj8a", 1186120 },
        {"SiWo5k6msAbdmj9GTDZWdHtRUnTjdz2jtP", 1186120 },
        {"Sdsn6cRW6d8YVEVmKFYxLdyhcEBEE9zsib", 1186120 },
        {"SaJeXNMUH2zk5iNsKAKx215DGYoyEGPvD1", 1186120 },
        {"SWQ3B9oJe6fUsTiWHzEVot2q2bqofhP8t7", 1186120 },
        {"SNiAku3mza5Wamyi3i6WQVJKM4abJBq43b", 1186120 },
        {"SUtDC7ycuPxyBiGsob8Uc73Ni8sPP7bivG", 1186120 },
        {"SZEvqFrF3177P1LXKjmzMSn66v9JLVj7vT", 1186120 },
        {"Sae7nonGBf2pqRm7bPSrBCe5QegJHEH2YK", 1186120 },
        {"SQGDBiGpCkAjw7TYRjGX7tdpfJw2KiM2Nn", 1186120 },
        {"SSqt8zf9wNicndq2WdN3eLmY6VQUqPtifm", 1186120 },
        {"SfcczLQpftoctxy2kg2vRo6LBdZyftmyXT", 1186120 },
        {"SNUTUD2cgSpVXNyR1bH8xNqL9gehT3NHpK", 1186120 },
        {"SPLGAyD9KBw4n9bTjTbpUAZ5MPBBZMPzgm", 1186120 },
        {"SPRXCNghq6gVtPfXa1kxQQHbFLqFudmEdm", 1186120 },
        {"STYPRXUmPdBLRiGJtVhNgFHiaGcZN818MW", 1186120 },
        {"Sj3WEMXDJinbbWiD4iGkQahFDRT69H2vWA", 1186120 },
        {"SXBBh4P1FYQALgADCfjzzzed4F81cGbG4g", 1186120 },
        {"STnhHf8bS9DkvGVuHYx4qghLMCcoDgk4t2", 1186120 },
        {"SSfXVn1Ujs7Xap9raaME8YQr3MicYQpvAA", 1186120 },
        {"SPAYHqgTqJYB7f9kC9tcLmt1nod4VQXT7t", 1186120 },
        {"Sb9KSN4BbU7R33KdEWNJ4vHHXycyx8vJgJ", 1186120 },
        {"SRhxwcVVhTMU6Y8MJayguNNuFpyD7JN7aV", 1186120 },
        {"SSKWLjr6qPFV735XpJvcz9CDtCF8fQ8M5e", 1186120 },
        {"SYwLSy3WQrHyAxEMJgB9dD4xSM7oTt6c1k", 1186120 },
        {"SNzKhjcmXwikpoA2NqAm7YfdjYUyDmTivV", 1186120 },
        {"ScqhqvP4TMfEwEACccnD9w3NQaKgq8kPzS", 1186120 },
        {"SgjRew5ZJ5HhnjtWfUVJrUzVPFqvhWqDAc", 1186120 },
        {"STHTTdAscHRBGjXyH64FDgP8r4jGQXuWVh", 1186120 },
        {"SQygnUFMC82mvtFM89fGRm38TDN1UPHBbb", 1186120 },
        {"SRtkH3AttrAdojHVbYpkuURWYhGRjdUFNk", 1186120 },
        {"SYdBrxNwgjMmyTMQKkua2gbho543ixPtd8", 1186120 },
        {"SakF3ZPhidLcH4jcJ2WFdoJVaMFRmGGc1e", 1186120 },
        {"SQxZLFBkNQZFMxP5bYZ2qxDL1Jt9xiSSUv", 1186120 },
        {"SUVjTXy9qxEdhmTmXxd56VUZ4zDue6bfzE", 1186120 },
        {"SVt1f3A6XZD3KQHg9y7ncxhGLHg4YtSyAG", 1186120 },
        {"STbPszv5BKwMvyU6AD1CouzdVESVMenepS", 1186120 },
        {"SNXwgFSrnYmcngZ2RM9aoXbzCBzaNGR5bC", 1186120 },
        {"SYhrWe55YsR1NdP2NR1QSjwv5fJnSwfSu5", 1186120 },
        {"SakssY8EY61imTH94dS3XCrLKMx3RWf7nt", 1186120 },
        {"Sc8iRKi9deu8N1dZvEkiV2UYmuxtbLPdWx", 1186120 },
        {"SfL4LX3iLCG9JjYpWuSufByS5osYRRVwvX", 1186120 },
        {"SczBy7wYsubVmSrHn77431dYUEogdK8CJc", 1186120 },
        {"SfuJyhyYEfynHmaLoR6atsc3LCQNcAkrpU", 1186120 },
        {"SgRgXWZ5KLV747r36qMd4AgCYRTfqJ6EFA", 1186120 },
        {"ScadgH3jumESKWKCiS6p6LkjG3KPiZcCD2", 1186120 },
        {"SPbE3rciEYHNQUmvBoVKNxM5Hsvv6uGXhC", 1186120 },
        {"ST89neCz71Ph5xCb2xErfsKpf4kS9G8izN", 1186120 },
        {"SaKacnStGczSbXVDRoqVdPMdQmywVzmhTk", 1186120 },
        {"ScbWzThb5cwbtFzSSTY6XEgNR1XaoMnKx7", 1186120 },
        {"SakTWuUN24XJ5HmQaUFP7aSnD8uWUVgXqS", 1186120 },
        {"SXLrtcgVb6CA5bMMiUWgvokK9H3JsdKCGX", 1186120 },
        {"SMvJ1T43NjStNih2XbBf2XHKrfGQ7ZVqAb", 1186120 },
        {"SbWwJX38BfdnY1NQxfSxB2aQHzNfjaNmfM", 1186120 },
        {"Sj91WDbeFJjExr2r1Xb7NZLr5Pj6x9Fj7T", 1186120 },
        {"SjJfWcw2jT866s8QX6r76hPuTya1qhPcwp", 1186120 },
        {"SSHHirucgW79BWiE1guxhQULHTwQrwfvXk", 1186120 },
        {"SZHdj4MwCQ2Hff3XXpMPzh6iS3CqRo3sCu", 1186120 },
        {"Sb7MEAHWXd747LHKPDKM93MCZWdT1UAUVx", 1186120 },
        {"SU4BcQmKVFSGSTRgHsggq5Xs8KmCx2MQVy", 1186120 },
        {"SNjYqcyhQLnx3Uak7UPggv9MJpKkRCvnUT", 1186120 },
        {"SYPfaxzKkroMBsUtRiHjyoDBy2sa5EtNz1", 1186120 },
        {"Saeie9UwqQdp3S2xna3XdcD7mjMJ31tbUv", 1186120 },
        {"SQ1jY1tjHD6GaLd6rFfxpyqxTa2bwnHuLT", 1186120 },
        {"ScvmcHcWjMc8xymiXnMGmcN5bs3AKFqTnw", 1186120 },
        {"SPAhjNobmRBc4zdiDngQgDYvJ5wQeb1mTJ", 1186120 },
        {"SSvv4rj1iqqGtSgBB6adXgHekRnWcz3scR", 1186120 },
        {"SbL2isRLqzwLRF6deFS8ozwWkni5Bs8S5P", 1186120 },
        {"SUvTENGZgq1rTZ5y6GaePkTrppSF23pwHB", 1186120 },
        {"SdDsfNgtdAwaNbX6MXw8CHE1QokRddb1kj", 1186120 },
        {"SiSHaK2NUbw7y9umJRsjg353N97Mf6waNY", 1186120 },
        {"ShMb4kLifwyu9HCAU1j4KKKSaMo3HpoX5H", 1186120 },
        {"ShAbJG4xX8DY6dxP296UKzT7Mw2LvtStsR", 1186120 },
        {"SdP7xaMQB77f6aPMVERa2sNxQVQRrZVGbY", 1186120 },
        {"SR82xfMfygpzw88dR2AfYS2jWZTbZwzbvm", 1186120 },
        {"SNjguUHTpp51afdw8GeFWkb1mJ6AZVTvra", 1186120 },
        {"SQrkoufyQpDjrfCA9D9tx6C9JHziGUu9xy", 1186120 },
        {"Sc7DXC7yGq7rTEAutHHSsUuLgcc5Dr7Wmb", 1186120 },
        {"SbugBqLY3us4zwe2xZL5fpcyeWrnsLAhfw", 1186120 },
        {"SNxR4MiuVhnuqVdudwgdxYaxW3KkEpCCjd", 1186120 },
        {"SPf7ob1JwEBnbFFWm3uvYB4Z2yE9EZiNuU", 1186120 },
        {"SdxEXBajGNDFm5AqEfzFirRs2a6e3N8H9p", 1186120 },
        {"SVNqFSq1LioD7fUyz95tgyujy3PDSYeysv", 1186120 },
        {"ShexCNXW78NRKY2GybfEznm49n1WL1e5dg", 1186120 },
        {"Sfkv53bJNcQPTQnjdC747wUrbopx5fDBan", 1186120 },
        {"SXTkK1bGo4u9oitdVYHbD5Cg8LTWwtyngm", 1186120 },
        {"STAjuVLWWkHjnDRiC5KDSkzpHYWhhsY8d1", 1186120 },
        {"SWeUNv5GVXrReKzF7WvooaLLGvRwRVgW6H", 1186120 },
        {"SQbFGKRk1aRqfVm7gyebfBiXRrdkhCCWuj", 1186120 },
        {"SSahP9E8gdqTKpjxGphFW56i4jR3bLextN", 1186120 },
        {"Scb2fpikvcnrtr8jZcsVZuLfqFspwjqUb5", 1186120 },
        {"SRzq3yA7gGz4XNvnKQUcE3bg7nDWMdVSqb", 1186120 },
        {"SX3X1nLyw3ktUu4aJxEodnrtQwKuXqbiD3", 1186120 },
        {"SR8dhPr1oGPz2M8pNAZKMEC2HYDJXBfHCT", 1186120 },
        {"SZHoXbkv9Mq7CCywZ92sGzFcFCYe6f2AGt", 1186120 },
        {"Sj4QCpiERQzjUfzVQhH7Dv21ZbvnhnuKBY", 1186120 },
        {"SWLJaSDgBYsZ5ryRDLMSdZXKorZxwvQys2", 1186120 },
        {"SedE3xR3Vi3C8RNzJq2Ht1zapZLYWoZ7DG", 1186120 },
        {"SYMTJMXkUHZg1JviniLU9bgeJie6VZGCJm", 1186120 },
        {"SVYc8yVDnnx9g5F31spL4dHQJ2yPirJBgB", 1186120 },
        {"Sdxge4ZXa84VahQyCrJFoFFezWDZJk4YYq", 1186120 },
        {"SPnkQrjhigAV3EfBCeydYUW92fUnD9DoxL", 1186120 },
        {"SRmjETic4CN5PxrT2KeqGx3TPmevKvce8w", 1186120 },
        {"Sc9B7fJTUn4dDvz2qgCgT7Zq9WpQ15gQbR", 1186120 },
        {"STD1jZmWzKWmwrXwqokaRrJp7furc6Veh7", 1186120 },
        {"SbyVUv7CHwywJ68KzfNycBjGA7Ko1yNQ8p", 1186120 },
        {"SZnL5NrjfZpaFmZxL8211N1Aqn6PNGzTS3", 1186120 },
        {"SeQCNRoFL57sXwreW8VztpGCEoQaM9pCYz", 1186120 },
        {"SYRvycdQWy6Aq8cFrZwvr9jd4gH3JtSgC4", 1186120 },
        {"SaWaSnaooCE7eeZuQeLCcGCboSn1My2ies", 1186120 },
        {"STF4LT1FYJuMrJCzsnTEGWpkQsX8anfnD4", 1186120 },
        {"Sc8vWiXeo9DQeLH8op82BEDZYMkjeyV6CD", 1186120 },
        {"STqHeGR3xiXsG1mmsQzNBcy6iuCXHizuup", 1186120 },
        {"SioAtSPuxEWbnz5pqQhKMqdTtZGT2bhXR8", 1186120 },
        {"SkXiMFMB43MMUCaD51BgvQACmfnuHyQRiK", 1186120 },
        {"SiqawrJpieQxSCoKDZ8mHyK9D92MTP82B9", 1186120 },
        {"SNe7Q8KoKLTwcT7TtDEMBfA4BB7dWUQybJ", 1186120 },
        {"SWqLhTtmGczwTyTBXkoPABrs7kyVbPCVqu", 1186120 },
        {"Sh32wTwCpwHeAw8GfDExenbmEJH98L4RvP", 1186120 },
        {"SfgsxJw31mD8nTazELqwn6dH1pmuASf3tJ", 1186120 },
        {"SWzmHCJuGTmJAm42RGJMqNEpRi6q46fJQP", 1186120 },
        {"SaFefWrWLAAoGDVbuwQUHq6M4Jq39izthA", 1186120 },
        {"Sb986QEqwEuTzVCE6f4s8cxgZF3KPZ2Ls8", 1186120 },
        {"SihsC9GtGcqZ8XbZ9pWZXUcAWpGkTLNdy6", 1186120 },
        {"SdANwKDyM4tjmSnJJtzf7PTNfnM1H2rc9t", 1186120 },
        {"Sjbj9oyCkxfWuLM2jc4BxnwdnJfHHtgaiT", 1186120 },
        {"SdT8gg6D5Wd6QD8on9VA8A52gVFqeQt6fx", 1186120 },
        {"Se8EGh6tjueirXhddzHh8YqU1JJDbdhttT", 1186120 },
        {"SMdkurtdLdkwdrEukrSF46e3L7fEjYViAg", 1186120 },
        {"SQRVM4WQ6sLatnSm6PcaD12v9F1TdJSwPk", 1186120 },
        {"SMvnMw1Xb99n1LCJEBT21URJbiGJkwiPUs", 1186120 },
        {"SZK5yE3LTUTo1PTYnUoE3aiMekUdVhUJrU", 1186120 },
        {"SYED9e9Ccc99PyxwMo4HtM9czdSRbqM3wA", 1186120 },
        {"Sif1V7CviNVVXz1xVVsJwjcU3CmaYSrc5w", 1186120 },
        {"SY1Jh9mcbZG5R2y3i6pBcGiURTLkLMZYwS", 1186120 },
        {"SZYxpCWmJcFQRobkpx2Zw668fHzxauUQ6i", 1186120 },
        {"SUJtExZtfcC5W39SBKYcc1b2xb2GWP4Cww", 1186120 },
        {"SP697ENiFkB7z6RRdFr82kAePnzeC8UeEh", 1186120 },
        {"SNgNe9BnjM3M4Jq4tSVUCNxxsB7pw3EZH8", 1186120 },
        {"SXmEZ1mimLKUtqzQRfDup6m1LgsNTV9J7c", 1186120 },
        {"SMtbpkGLAGBL96Jad5KfsETtsMy3H64hZ6", 1186120 },
        {"SkM69bkKtYSUdxdXpirUpetuXedkpEdVTW", 1186120 },
        {"SaCyg5pzoJAeQqnff5E57NqcUbV4TJ2UxC", 1186120 },
        {"SUvVng1Y4y5VHEESa1yokoub4Mjy6dywF3", 1186120 },
        {"SdLKUjQN3i3tdshyrpLsZ8VCGvp6WUEb6m", 1186120 },
        {"SVvhh4PiSjYqgj3T13ZDqnh15bnJPCGUCp", 1186120 },
        {"SNAJ5uaEz4DrgrkFVjoyokF6cgBjHTEALa", 1186120 },
        {"SgpX8MfjYow7b3qjS54vNDC6TnRsTTGqPX", 1186120 },
        {"SNjN4UrrQTUtaMbFDE4EZeKtJ5611EST52", 1186120 },
        {"SZjGcd3XUFPDkaoG5sS5eYxQGnDaw9DG9K", 1186120 },
        {"SdqoBxWorrsNAZgjobyJB9m1p6hN1GhRSc", 1186120 },
        {"SW2TLpvYT8m8pwC63yybZFgH5QHf7yXcG5", 1186120 },
        {"SYDkQyi75vBsHyj2r55tLxSFPq8VuiyWca", 1186120 },
        {"SecPdyK1bC139GJzc17NKtBzXxXLAFfK6w", 1186120 },
        {"SbGJoM6Ha7N1FvskYxhmjVqJC5bi5fXm6y", 1186120 },
        {"Sgh1wbtU5NmmuhHma7zzjZ4akNtuAZr7bt", 1186120 },
        {"SQk3YEWDmv2GybHWUCzP2iqW9wfSLNqsrU", 1186120 },
        {"ShoGqsQrVB4vB3G5JwTcdVBo8G9kpLqXrx", 1186120 },
        {"SiVvvP4rVXakvxYQCZ3tNZGN9WTqeqWWdb", 1186120 },
        {"SdhiidXi2sTZugXw1NepRgudajGu9covtT", 1186120 },
        {"SX5dc7Pj39kmvmz9FTrTvdFXf5Rp6yWvMW", 1186120 },
        {"SfHZs78B9ekPK5koiGW1kEACzGGk1cq5VH", 1186120 },
        {"Sc2Rg8jCwnBLk3xH59ky29XZCDqgmD6Gde", 1186120 },
        {"SQ8E5NvJhCrUtwfiXKGPsCuM5FJHUfTXxQ", 1186120 },
        {"SZ5wkMCEfreJVExwoD7PG9bCNg9BYCuWA7", 1186120 },
        {"SWMVv386ewWqjBiKn3Av7HNB4WHLq6BxyU", 1186120 },
        {"Shvm5hkrT67dQMzL59S9AbwH3FQDkRJDHB", 1186120 },
        {"ShX5S7xeJFJZLGUYrZxQG5CwR7TqQn2i69", 1186120 },
        {"Sb8qb928fTe7XzDbxY4pqoKprCY6QWW7Qg", 1186120 },
        {"Sau2R1ipXHvjiysLSNEsEcstzMbT9BtQ91", 1186120 },
        {"SRfnkdkF5WofCpnsksQWgMFxwuvucJVAfT", 1186120 },
        {"SjArLTUqKFSygvVNYH7Ndt9TVVxNkLb4ev", 1186120 },
        {"SewsGcg1Ac3WZbiQDgSbPe6Jbt4j7ERajB", 1186120 },
        {"SQbUm3bishkVsxjSJDPYDbfEKrTT5JLQao", 1186120 },
        {"SXpf9CHJmRypf3H1bkr916RoFpGbnLkUef", 1186120 },
        {"SPz36p9fsfkZ7mmcepgzktPHhRetsVVeer", 1186120 },
        {"SggHJdiJbqQQBANTyAaTNLcJdUYX6iFSTK", 1186120 },
        {"SUY13tiFiAdCT3V242mypMQeqxByW6U7n2", 1186120 },
        {"SWibvdpqAzHg9qvNRcq3VGqyU7kyMNtDKm", 1186120 },
        {"SXonx7rtwwy2imuS6C7HYFfowEfXSKN7Yf", 1186120 },
        {"SXQShoXkrqkDHTCmpKM43XfxgHFSPzsK1V", 1186120 },
        {"Sk4SLhpqnLMFeiwjVrroXFxfhqs53nVG9u", 1186120 },
        {"SZ9twugTHyrVbeLuS9ant3ojq8cQ3uNigJ", 1186120 },
        {"SVWqaDoTkGcXvtFY2M1MeUxYG11Fwrox89", 1186120 },
        {"SStnHDCNtmCMbkrLyxzMAsgMZPfZgnyZrf", 1186120 },
        {"SiWGV4RBCLN7FQpG6QzDTDKEk6vaPKd8af", 1186120 },
        {"SNHxJvSDqxAnsowaT3B8BadmHgFamQsyyu", 1186120 },
        {"SdDLdMCcFP6RWMkb6wsp1uDDY3rod7PFPa", 1186120 },
        {"SPe81zF6S1J1GFLiDptrSvvSMExJyifKW9", 1186120 },
        {"SQz3PQnV1mEH8rhLV3gbes77JtuL5YPpQB", 1186120 },
        {"SVtEFLQ3JsnedaA85XpEpS8bDkgMmcRfhk", 1186120 },
        {"SU1ci9gWSEt9V135SqAmUdN7T2gQxkVVoa", 1186120 },
        {"SPadiiTFuY6nh81Ubj6sNj6P3YprjNLiBy", 1186120 },
        {"SPMo4Att2yLohitWqMXcXbWGDi1d4ahggu", 1186120 },
        {"ScCE2XcYoh2swtBfNeu3jZ4q6QGx76ro2v", 1186120 },
        {"SaYzn5i3Tu94x8Qrt6zMaSoWxiHX8Z2Wvv", 1186120 },
        {"SU2i6nJKE3SYWcfsMp2QYnbiQAnpT9CsNU", 1186120 },
        {"ST4ZzfvgvLyRhSHCMKgX4BeC7u8gobpoGc", 1186120 },
        {"SdT4aRrXY4gvFvcugGAo8aQNinwpE9YEXB", 1186120 },
        {"SXVh5nafxHxgeVmd7jyz2mqYhHwNRtsDgL", 1186120 },
        {"SeKyvmRBWa7FqYwQvvnp5D8arspz9v5nH2", 1186120 },
        {"SZjD9H62gjzYAqRjsgpJLYqRJ1St4LtLby", 1186120 },
        {"Sk7h546scHLrC7xm7royVhF3Lm82MznAzy", 1186120 },
        {"SjNtFLdMy9YcRFbfXBYCSfsN2Ejo3jXK7Y", 1186120 },
        {"SSFQvrPvzSjQz23WVbAHguRqMTDPU3Y52F", 1186120 },
        {"SN9h9jd7Ybgka7xpmaEs9jj3iHugbdTLLr", 1186120 },
        {"SUzTAzwtyij9TLXfc9EKZ4MTw99bfFfzXX", 1186120 },
        {"SZmuBKNZWBUynbYN4NeYooMijpcboers3C", 1186120 },
        {"SNmTRWDH7ZrXSViHySDCowbohGijjLwB4d", 1186120 },
        {"SW3seKFizB5dpPRiAAaEJEfCNdGrQANtTs", 1186120 },
        {"ScpsktCJNVCHfHdp5xJWuaFRhbzFohir4T", 1186120 },
        {"SU5ATpfU74tm2J6CQJ5ZZMnJkY6HaHMaip", 1186120 },
        {"SZgoQChQv1vmCmiRSCp16w6nrpYTKhNURD", 1186120 },
        {"SQbWbu6WC1SzshKLUf6CYy1CXpXE2wrtH3", 1186120 },
        {"Se84FZDDC4vmtpkScNrYnEfudHJKDBuyW7", 1186120 },
        {"SN9u6g6nuADVKfBgWNL3PgaAWDDviL7Hir", 1186120 },
        {"SfoYMipaRyTbYaohZjeTBugLU2UxT419Cv", 1186120 },
        {"SSts3Dmtt8VBXRDmZqzS5oWvDmZdKSaw3F", 1186120 },
        {"SU9NjwBb4U3ycJfMMh4KMibc4zghSoo53U", 1186120 },
        {"SY2UGG9ZzegfkKdJSg1FmZUR86S4gYSiEw", 1186120 },
        {"SRiwipcgrQoDhPHXmFyCCuhxSiRnsYqrYn", 1186120 },
        {"SU3f5S6LRhnmtEfQYJvkgYbPeZ5psFtzx2", 1186120 },
        {"SUgkY7dhHr8RVLHEBJqGmoBBsRYM82jMHT", 1186120 },
        {"SPM9xqBijS1GLiMfCAnXN1sWZqt45XYBZc", 1186120 },
        {"SZmKPAhTqXC481gxBgifpJNzP4rAmvZkLV", 1186120 },
        {"Sf5a1YHLWZrNF8LfBwmBqBTdPGQa1oSXBX", 1186120 },
        {"SQBA7RKjoVBbXX6pouyzpKvDJQr8eQjvXh", 1186120 },
        {"SU7oZ1yVxLAEoT8XyK6YCrLCojjCAFn82o", 1186120 },
        {"ScLMgGemBLsK5CLWRnbRorLqAwjH6DZ9hv", 1186120 },
        {"SfGB7PJxYSYoHKgeJrJKRk1cYMTzp2mNwf", 1186120 },
        {"SWh3EBiRxABjm9eJuGgLNydA39BMDcYLC9", 1186120 },
        {"SWUwohAb5zbQUFSxQrsjsKsH1VVknfaDNg", 1186120 },
        {"SRCDCrcbYdmnqJ6Aa6GXXNDhxbYEQXMWkF", 1186120 },
        {"SMQE4JV9bYSVoRnu6nWiDDPtUhxHeVLeTt", 1186120 },
        {"SNi5Jdwt6imxh8snsMMwqatwDasDorZqkp", 1186120 },
        {"SPx7WKBaDxLgZQrzHVVMGRLBh4w6p4xCJR", 1186120 },
        {"SgeDxKc6r9Bq8koYDz5oQDu9RyM1NL6RCF", 1186120 },
        {"SVhjnn5suquxMmzGFFsiLvNpBaTGuJnr77", 1186120 },
        {"Sj9kNKPBR9wCAB2xM8qMxyJa1YqcQi9sQR", 1186120 },
        {"SSEshQJGbisr6DpwdYPDaELF5QV2fi4WjL", 1186120 },
        {"SQMLpPQsnTzmGwD4SkQUF8yiQjdSozKgAG", 1186120 },
        {"SRm7rRhkznQDf1erCLbt6hNG5TcSbkqcte", 1186120 },
        {"SPEtuSYgedMtNLrbt7nxq3qQd6LNtM3AdW", 1186120 },
        {"SSVrsov1SFpMhrMuLEUfXqYbBzjnC1kUmu", 1186120 },
        {"Scx1Y6aj7GVWRP8df6moqnnfevYZY1ChyQ", 1186120 },
        {"SZzkL1SQX5hC5V2sPDDhvbcwXSSF7Av9BL", 1186120 },
        {"Sfa1915PE5KwTRoL1frDZddnfFBqJ2bnPr", 1186120 },
        {"SNZR2Zn9i9grV9MWDj6ggbyQtrosKZ8iTk", 1186120 },
        {"SbHbR5Jn65Dfdir2aCCQMTKaSG3297qgoe", 1186120 },
        {"Sg2HxxkhG3yRzHW7NqUKrnUPotmU58HdRG", 1186120 },
        {"SiqiGnCTBo45rPKcnxHCT5RCPuStQB2Jng", 1186120 },
        {"SW78vUm7gcziFtxo8jivNbWGSgvVU8QGLK", 1186120 },
        {"SUT8uvmBxT6ty746shnVnyisrb1Zv44XGm", 1186120 },
        {"SjM2b2ygpFFUoqzsED9cNcqdj54hWaXt4r", 1186120 },
        {"SS2MbU2fZucxZ5QmVBZ6xtn8jcvFTmkat9", 1186120 },
        {"SSyaHvfZs5A91sYoxXhpnvV9EcARiYtHVY", 1186120 },
        {"SkF9MnXWFMoHm474xkHXDM6Xi9gBj8SjgZ", 1186120 },
        {"SW4r8a28x8YJQQ1XUfhfpagm9bmeisBnkY", 1186120 },
        {"SeWMbykQtLw1jJEa6efW1gzG1ri5KVcJ89", 1186120 },
        {"SdXM8gXb5KBe6uoS7RZ3ura2suGu37647E", 1186120 },
        {"ShkWNFxFzsrjiXWpcko9PdMXkvxavfALgL", 1186120 },
        {"SaLbrQDha3acEYScZb59KS3j3XPqf5FbG2", 1186120 },
        {"SabFTdHeYM9o2fed9RgpAHcMuKC9Ymp2Kn", 1186120 },
        {"SciEDniCjPKgvQpYgWbhyuLnzzroZmpWiF", 1186120 },
        {"SSiZbouLeFPUc2C9NzQ1YNMQwntQygaJSy", 1186120 },
        {"Sd2MsFt18EmvLHpTuaWaegmebrfsdA4UzH", 1186120 },
        {"SVUBVFT3EkmFqbyoZ1FFLHaNv13oaawJ5M", 1186120 },
        {"Sb6K7GCJ8RGGXqeLzw11rDMvymB4Qtce7J", 1186120 },
        {"SWJh8kJfpZBLoJVaManVyEZjT5tiTd9FSZ", 1186120 },
        {"ScSryqsTEovkEiLuymyFueCCj2hScwvckZ", 1186120 },
        {"SabstMJSiJhH4fPwnAjMbwZD93hTeVktFQ", 1186120 },
        {"SURfRADz3QNChLCNYXTiW8xHyMqt8tEps3", 1186120 },
        {"SMJ4ez4bZkfVocCqX8vnT9f2uqofXMuuHX", 1186120 },
        {"SdsbimBy886VKVbFNcB8L8nqvBLtNMenFP", 1186120 },
        {"SiRMd9wGiKcwnTR4FxPkLxfoBT5t6bwdPo", 1186120 },
        {"SkPxPr7QQLctYRmRqwJt4jjVy5tefxN7Zv", 1186120 },
        {"SYjHD8QQiNJjjeeMK4A4DgeziXoM9Vfweo", 1186120 },
        {"SQ22adpZ32BBHJ8bVZGj5ULadiqZVmdRTi", 1186120 },
        {"SMShPTnipiCtdh7L7mTTXAwBggquJRcUuY", 1186120 },
        {"SSp6F46i7Y5ZGK5BB9XLjGzCiXWhYZGeK1", 1186120 },
        {"SXApdtbMWBWoXJYMLQzoWhjm3EQSCRW9h7", 1186120 },
        {"SNJQDhF7PGRCyeezwX46ctwZhWBbr6CP1w", 1186120 },
        {"SPQ8FH13jKoVWc5UHdx2aVKbSbu4gwnbd3", 1186120 },
        {"SMpKXqjhFjdZsb8Q8rb8XiJkxuSUKJr2sV", 1186120 },
        {"SRKLpRgiCMGLGytoCBYFZE8eTUGY4ez85a", 1186120 },
        {"SaqDXdxrftbwbFyjpbJwUGaiSuGoAE1N58", 1186120 },
        {"SQoPwXRv4ivSSLi5WVp5ywg2yNST8NArX8", 1186120 },
        {"SgLLXTQACo9w6BbKZjTEw3NngJ2EKbLhqq", 1186120 },
        {"Sg6TK9oKJ5HCHvmbjNczxb8XJdqN3tzuNK", 1186120 },
        {"SitL7ifS9GA5iZhJYm3wiwaF4QeBs8gE5N", 1186120 },
        {"Ses9Ctbbeb9daM6qQTjs3qfa4APQqFAAyw", 1186120 },
        {"Sc8SwPQWRZuX74CphezFPQHDnEX33Zof4H", 1186120 },
        {"SSGE5fvX62pP1XdjjydHQNEq5Agrfijy6L", 1186120 },
        {"SNQrJ3RGUsW3YkKXKDALJZke1g7xSkrLRw", 1186120 },
        {"SbVKm1bVfhC9AbFoPAXh15MSfjZtqfLPDt", 1186120 },
        {"ScxA7LyQzbCE6wyEp1AnoQxzcCboXrcZum", 1186120 },
        {"Scf56YSp8WJ8DJfg22nyki6VvAPDJUaWTT", 1186120 },
        {"SZwFjoDmQZnuQ5qzegnwxFxvG6sJ78vSdb", 1186120 },
        {"SQ2kRXwjMzxUjzgChjHVpYuPq8EesyGeJJ", 1186120 },
        {"SWo8BP6kH6BXZVkX1v9bwZZak4kkjbdVcg", 1186120 },
        {"SZnjUnPZZPHiZMXJJnMvmaoBr9jYHeVNep", 1186120 },
        {"SfoZvaATUBKFkebtk97CBGTUuj2FWxnK8f", 1186120 },
        {"ShV3WtG6tL9B47pPCxDBELecCPvdz2dGcD", 1186120 },
        {"Sb2CHWbFsgZPNGMXrGqMrusG8teDmUcLrB", 1186120 },
        {"SW73gHFBnQZc2bzq48wPdRYryLNfi23e4E", 1186120 },
        {"ShUVzWTERkrTGTy9tx1ghQjFeLS8cbhbHt", 1186120 },
        {"SWoP3jKi61VrfPSMUFHouTNKu8G8HkWiuN", 1186120 },
        {"SXiBh1YXzSoSKenv1XuUPNPCt123K56RAc", 1186120 },
        {"SkMQB8634yYcc1GLQCdoaErqaEWxin7q1m", 1186120 },
        {"SPTrF9NiXSLAM3NdSur2XQZaFhkbYqmfcB", 1186120 },
        {"SYhkxcZKQvDgi3tNfr5dAEyqjyXiBFKdya", 1186120 },
        {"STMy2tu4X8WEjoD3Lt5KwXW9XKkx8wa53S", 1186120 },
        {"SXCW6VkuYxedEy2g2ic7ctSRiU5nqvpQ1G", 1186120 },
        {"SfxrkSnHzQjLvuzzffNfi2GaFtBc9s6Dq4", 1186120 },
        {"ShxPHxov5zwtYW1VBNaDMzjx6CYpjBwxxA", 1186120 },
        {"SXLFzzBx2HeAiTuLooMpby7uHdWYJ7kkGk", 1186120 },
        {"Sah3Ku4gxvjBMAnkrRmrkhynuvqSWW5TK5", 1186120 },
        {"ShnzN9rYjLnHoBFVcLngUoQSiKfK4GwpCp", 1186120 },
        {"SkK5gMosEZ38ienrWHm1y3fdCxK5K8Jja6", 1186120 },
        {"SZ1RkgapiESbq1w65rHKRobRhJF57StJMD", 1186120 },
        {"Sfo884NW8UmdyGzW4Qii6m3mpfzfBT8f7C", 1186120 },
        {"SU6JGDoCRjge9Z3m1NF8SbrHtFJcxqEHQs", 1186120 },
        {"SYEKTmuF8Dgd2pULbfC3MWj6jZUBWQ5n9x", 1186120 },
        {"SXiPBmYPqzMbNSVVg8d94V8oPQooQT7Gso", 1186120 },
        {"SjVK5UEtPdY332KtppLYFPcoDLVWhTwJjc", 1186120 },
        {"SYoKcbMaGxKwdc5DAbxQYAp15fD7fphSjx", 1186120 },
        {"Si6qXoqLS7Cca42GF1ouGJQE5vhdGXJss3", 1186120 },
        {"SkcFJt2CD1bLvgFTJJFdxtj3uawfBmh9LT", 1186120 },
        {"ShjigBisnWBCGGpPqbExCbmDXrL9uNV2YD", 1186120 },
        {"Shcr9a7HTrDZGJCaxgvKgJQPRU6dc4dp8G", 1186120 },
        {"SPLz55SySne1W1UX9QHBcpQQdNgvJkiNS5", 1186120 },
        {"SkGMEBXURFsuT8op9KiyNpd7xJ3D1Zd2kc", 1186120 },
        {"SUE26rgc4Hbfm1SAuA8XXctGrHF3xZNuwE", 1186120 },
        {"SVu1JGbPjmPTGyZN6witp6hDdgnavTqFhb", 1186120 },
        {"ShYaJHmXeKLLUjRrKNTLZc3qy3kLGD9tbe", 1186120 },
        {"SRQbJjGLM3TSJXj7pYtcTtYbYRzcz34JRG", 1186120 },
        {"Sco8BgHNA72L5mmJuEjrfj2eSyLbuvPpwy", 1186120 },
        {"SNRbGWDq6quAS9GQG8cL1xLB4eTmESnpEL", 1186120 },
        {"SWdJXFbDQLDgcw38EhbBxxJgjXNScjZZu9", 1186120 },
        {"SXFc4FygHdT9GYBGcwkcT3mSECcUo6rUmg", 1186120 },
        {"Sgptvutvk4bDZ9PHnLj6GjxCrk1xAoDFan", 1186120 },
        {"Sd4qBqf7TLwYsFP45VB3CkjyQNEQswYAKT", 1186120 },
        {"SNXkHURrrbBuVHcxsRVn8r1EYPAKvUx2vH", 1186120 },
        {"SPBx9KnW9wmaUh8HyeS3Ut4XjXBqYGeHKa", 1186120 },
        {"STGJDzSCPPHfZLGCRPa6PgQ4AuhsxQGA5e", 1186120 },
        {"SSQ2LTbjCnUTHYRFGSF8wJUq69qEMXiCmd", 1186120 },
        {"SNoxSW5XqxWRs5Yp63siXGvtakeWymnhHw", 1186120 },
        {"Saabo5wmpBrbf954hRF7MDNeymnFn16rSQ", 1186120 },
        {"SbeTLMFU6egwfjT6Vwm2LKoDiYB8f9gikk", 1186120 },
        {"SaJvDN2gBADwUXtmhJM8k8NsFNUYUM8GcV", 1186120 },
        {"SSutN98sFAgfcgbstZZ4rUzSMtszFAmNoB", 1186120 },
        {"SYz2exLAQpkGxXY8Cakyfgk6QTxCJKxPVP", 1186120 },
        {"ST84ArYXXnew2JNnc78dwWRSjZ3uwZmyJk", 1186120 },
        {"STNYZY5ztMYcUDUKDyxUQfEcJUx9dgF9Yw", 1186120 },
        {"SZ6pFa3TCF1Qqe9Keam52SY2fe41Mv2iAS", 1186120 },
        {"SW6f7J62xA89VypLM9bKQpDi3m1YbekTbq", 1186120 },
        {"Se2eNieGV9nNGALzMuzaEyHKTwCWxTxspT", 1186120 },
        {"SXWoo4E6YLD7XrhT3UZGfPZEyBpQ1yG4DS", 1186120 },
        {"Sekzh4F3McmmhgDYgtKTsTiDtkSWWmVLbA", 1186120 },
        {"SQnwVzHHqKfDJ1n2TPyMsqUtsbnM2p6vA9", 1186120 },
        {"Sied5DmCHDLYDkxjbmg1zW7VVUAVGXK83u", 1186120 },
        {"SgagkAoVyw65HQ5Mo3hGjeByB2LVXansgv", 1186120 },
        {"ShQDj7m6WE7RRpK76h6Cbdve6U5Y5XwNWi", 1186120 },
        {"SWm9DNEDQk4rJ92eK6LwR6novocxS3cb2v", 1186120 },
        {"SfC9hLVgTkLy1TqmAoUKUQfFjqrM6Gqkx3", 1186120 },
        {"SaLYrJz3CPtt3CFT91JiLEZnXE9amhTaq9", 1186120 },
        {"SjXRuDtTfBaRKHWKQSvWU6tHJRCZ3qRn7Q", 1186120 },
        {"SRzyvFJjWz1CvMHgzk8c7cRamXMy7Pn2MZ", 1186120 },
        {"SZcEEcx3FuoVeKDF6wxqKPiXPVavmGGSEk", 1186120 },
        {"SfYbs71pYHWhePnrGUVxhCBQupnYPbuWTU", 1186120 },
        {"SQvvQgFighP7m2dFzGsyZtqEWwecjrS1Vo", 1186120 },
        {"SVoUkgM67zYyHJKhy2wHUcU5pk9NBC3vJg", 1186120 },
        {"SSqTN2b1FCZs3GdRTi9vjPVRGPVo21XHyn", 1186120 },
        {"SSGUGw2iMS6UgdorqEihLHCJoivhX2d5oq", 1186120 },
        {"SQM4k7vmqKrnw3bS6Z4n4xxV3f3ny4rTTN", 1186120 },
        {"SdNNUctmNvTYMZ8bC3qax4jwj4TgzbWAHC", 1186120 },
        {"SRVEQh78wFdz6GVYwV62B7TA6pYnw2wmV2", 1186120 },
        {"SaNBroKNyLEK6fi4H28d3ixQqw18FSujyF", 1186120 },
        {"SWYws9Q4yVnscfmiXPALAH8T9VZWZTRMCv", 1186120 },
        {"SaqfsKnMQcWMMaRAvbg5Rq7MMUQPt4x5hF", 1186120 },
        {"SgUv6GQmWV77tHnxri2ptAaye79uERN3jM", 1186120 },
        {"STKLmqMhynwKjDsJJExp5DdVSFMgunNVMS", 1186120 },
        {"Sd6ceZQirhCDRG5CUVqhGqvhkJkhL9fTjZ", 1186120 },
        {"ShdZBAenoHSTw6C11XFFHDLxF6VmhSQBP2", 1186120 },
        {"SXesqmgYtRCt631hFumheM5X9rYmXgcM6U", 1186120 },
        {"SceebADA85W4vuULFzKephrWCvi9gxKrVh", 1186120 },
        {"SPJcCmRyyTZgvAhTJtbpS7V3F9SGaWRKA7", 1186120 },
        {"SYv6Rite1DHNuPFRCDMpyK5met5oQUbiHu", 1186120 },
        {"ScdmXgnDRUUSZvw7D53EyX7mLGo2YJowKC", 1245400 },
        {"SiBC5pVysUUkckqzVKfUewNDVEhkncZC3Z", 1245400 },
        {"ShLYQDheVGvpcyMRhsj5xJ1bo4w4jpsWLd", 1245400 },
        {"ShrGGD6nvtbp66jHxWDeRVL2Mxf7wFuPme", 1245400 },
        {"SVEy9XMK6VvxBSLE9hgQpq9nMaLsjbDirz", 1245400 },
        {"SPZQYEZQ2Ys4bpXQji6a4LhDxF1AELLJNp", 1245400 },
        {"SWyM3q2cnW7PFb84AbethNRidZ6zyvAjr6", 1245400 },
        {"SZqTBJ3UQZ1zQzqwK6Vb463nkWZWKUYKMb", 1245400 },
        {"SdLpfkohEfU5MhrM6cpwo7pEQDSJqzBkMi", 1245400 },
        {"SNxBzbrwx58dEGxjQBvKtehxMYRiLhp3bb", 1245400 },
        {"SedH6Qw2qj1FpUxcYWupGY5eiKcAXXEZ72", 1245400 },
        {"Sb41EDpocHEp1s9ajNEpneEnkbz71jzeSe", 1245400 },
        {"SeZETvLP1gC2EkiZU6C8RpwNT6ddZHcGSL", 1245400 },
        {"SY4AZWD1RqALGyfnPPvUUfv8Zkcm5eFoTG", 1245400 },
        {"SZGuhDU1zwL9Kctvex9P2hqnypFmsWHDYW", 1245400 },
        {"SjAwHgoZjWEULXgKcCmyfJyU6jWy5QSuXH", 1245400 },
        {"SNwEjY9NcYKvAPCF1B8srSV8wL9NowbVDY", 1245400 },
        {"Sd3AL5VtCQ4Uat8X7jdbY3eBVW5wg94Btd", 1245400 },
        {"SV2ZCSPaj9rQ2NLnJw3FymgopsNN3a4WEz", 1245400 },
        {"ScyBoaq7PRf2afdRSSDD9CzxzTQ85GJ4KN", 1245400 },
        {"SXjNRcPK9Tpivm7q6CfGHAE8ELCDMrhJqq", 1245400 },
        {"ShsafG1T1niMc6xTiBm8xnRAgaPo8rt1fr", 1245400 },
        {"ShVs57QyofY4KTpM5Q9jkGvw87GwbwuxC2", 1245400 },
        {"SPEVbv1iPsXsfr9abrnhWQJAa5HdzuSwuu", 1245400 },
        {"ScQVHSZbqA466GNuzajZXU92eZjdjGYF4f", 1245400 },
        {"SZXz8nSqdJXt6JZoCnorXRFPbRzjj3k9Et", 1245400 },
        {"SWYpJrriwU1WFAV8ABWkHjubjHMLsGLZLv", 1245400 },
        {"STGC5NCkUSga5iQFfPJiwNWuj9GJvW6qBh", 1245400 },
        {"SeGepD5PkxAQPpHKhr3nYRAXTwicD8RKbJ", 1245400 },
        {"SYovhm8Br89Rzx3P3Aq1kXWbxsKoc8E5jJ", 1245400 },
        {"SjcSnV73XqPDvMzgiP3VVytZzZuhcSmhyo", 1245400 },
        {"Se8eXZ2XQYinDyaFfRT5dSEfs8vtAFnvjL", 1245400 },
        {"SVc8u7pNdG6MCT1YnaP8sx4xrYTobNyFgC", 1245400 },
        {"SWSq1ZhbAnCdFudKpHYzoGGk9gHr4QhpPj", 1245400 },
        {"SUn3W5KGCFN45b69K8SMuKpNVphpe26r6V", 1245400 },
        {"Si2ng8xeqB65o3ipsWaw8xZ8V3fmCURDgn", 1245400 },
        {"SiSsYG7nisDqRDgP1gCw7tZHfUJa8NrsGq", 1245400 },
        {"Sas9cJAneDY5hGCyHj8BrapaZFxTmpjjPx", 1245400 },
        {"SViUz27UHwNUnNqSdF4GjPSB4z2sQ7tk51", 1245400 },
        {"SeJzL8WPmDL7jZfHyajSz6QHYw6nZQzVRp", 1245400 },
        {"SjKC46yUBqhgZgUK7fDxjNSphNZd8DX79t", 1245400 },
        {"Se2vzUFc5yQ2YzTVcyBetRRPNvXzN7dkxA", 1245400 },
        {"Skbp3hYgS1MDH23xUCHcyGt4s2p7Rh7i31", 1245400 },
        {"Sf9nD7CpNSU3tXgq33oi5fxm26jyfKyaku", 1245400 },
        {"SjG47VrZgdASkCYiVrM7JtWedkTYmsjbe5", 1245400 },
        {"SbLeJQYWWthFB8dqZJEwPMx4ZkxGLafFxd", 1245400 },
        {"SUqp3UZgofemuSBWFWN9zGhTJ6A5JaYBg9", 1245400 },
        {"SfGw4MquyeHqeCNE96jrWXNs3MoRKeVirF", 1245400 },
        {"SdbzrdoTwioEmBjUxouXCkjvnGPF4DSnFr", 1245400 },
        {"SXR1HGvCNG7ip22px1zgtAaWT6GZGZHzjT", 1245400 },
        {"Sf2yoScrL2J1sa9U5pAVeTq14XLsG8H7w1", 1245400 },
        {"SdhaBCEAw3MZoe1EcftSbvbuUL7CrMZSNR", 1245400 },
        {"SZte6QNZsWQvtt3MivXeHB91KiMja4cGkN", 1245400 },
        {"SPmXKt8C9yrzuTL69Ev51Jwkm2DzTrFcqM", 1245400 },
        {"SZCKnS6JyKAf3Ji9xQGBujcvxnYEovAxnj", 1245400 },
        {"SPSbdto7ajaCmhJyPDs7Q7BbxJy8UrDGqb", 1245400 },
        {"SVwGoEkTi8NR7MuUbnKUv857YM5VGR4gdZ", 1245400 },
        {"Sk5SQgeBHd3RP3Px3V9e2wU4xarJYmcME1", 1245400 },
        {"SSpE7PCTkF4NJ7VLNweBg9fUR7NWowhNmt", 1245400 },
        {"SdfmikxWa6mv6rWhKWqmFgUS25WH8Yrp76", 1245400 },
        {"Sacg6vmdVmtW2Fk5Q4XJ1r1uFLZXAyMu79", 1245400 },
        {"SXLNsVFcabp8vXEkdmZn977JLEzuYbjexE", 1245400 },
        {"SgJdiVqXXBq3yLjvd79A3vyrbBootB8HvC", 1245400 },
        {"SejbRDYeb4VTbfSrtvixsVWisdkY2XE5JM", 1245400 },
        {"SR5VinCdyJ2NaSNh1uL42pw9e2kuvGXceE", 1245400 },
        {"SZPaDEKiymA2mPZ4dzBegSdFGXs7ijV6bW", 1245400 },
        {"SYJwHm1ZvhxyiJvJozGHXGh6ygZSxxS5mj", 1245400 },
        {"SYeTgdBtUfhuW3kpDubLPCVC9sd9mkmvk3", 1245400 },
        {"SbHWjTQDontqETeAibfTSn554yBMyMCzgD", 1245400 },
        {"SZ4xpy3J9fsCEzQZ5ZSKJ47ycCSYUqmnWn", 1245400 },
        {"SdGijcwqmSDXY2q3BHuvGB3SmdPCof2me2", 1245400 },
        {"SWHCaMCWrQmmhbcDu24kA3vJmKpTUqX9eV", 1245400 },
        {"SRs8m7XchV9mBLpYiAeUcVN4daRxvboBTK", 1245400 },
        {"SZLcZsniPZBzazy2aBkNvQA8MAijfRzXZ7", 1245400 },
        {"SYEXSSUUQdLmnqNQZFCdBf2VHGfDZ2PRA8", 1245400 },
        {"SV2Sq3EJLXCUixZhMosTXvfRYbkqGKWAxg", 1245400 },
        {"SXw6tdHUmcFD8cVg1Chr92EPuUBVWECaQ1", 1245400 },
        {"SW2G6VAU8U2BcBVPZTBhBEwjaGXmCTE2kb", 1245400 },
        {"ShfDuVoSTbKPb24DaVVw3r7pwC9rKKpM2L", 1245400 },
        {"SjtMCe66U9bhtnV8YjRMJKPAjbQ5qCSgKb", 1245400 },
        {"Six5uH529AbUEru1XYKpAJztLYoax77YkP", 1245400 },
        {"SW4hhJzFSaTuiHPoPp4nZVpDJAwhffcwhx", 1245400 },
        {"SjQMZaFfLic5fMsum9siTSfcqNYFWTEnqD", 1245400 },
        {"SjSJ14DDJsfjoqz5ggRkXfYzqNLND1pzEv", 1245400 },
        {"SQ7RUpf2ZKFacx1JBGMe9EDMPgbLKvWEpa", 1245400 },
        {"SXScpHmfYRY6W91sDvJhzmxrH5L7RJ4A73", 1245400 },
        {"SZD64cVDjq6PcFkc9Vtd9XeFrw1dUgw5NV", 1245400 },
        {"Shs488psp3JoecYXwv4T37odwEzLaQ3UhN", 1245400 },
        {"SXTvxpFX9M51L6uhi3BUpCQfvGWg7okjJG", 1245400 },
        {"SiKCRErwdTR1PPutWYSMvMntxZMkDAE2BM", 1245400 },
        {"SaL8WirZgGRUYCfqhga8UPpvwGJnzyzfn1", 1245400 },
        {"SiQqm5P6TSm4Zyc8igAZMamXFEpAB7CMo8", 1245400 },
        {"STEZAKV6Upf87XukvtZiGoA5R5MVejnw7s", 1245400 },
        {"ScPsSa89mFGa3gukqNnLSeFBq5h3e5xqB1", 1245400 },
        {"SixgWuotpcmgBqZyJvhkeTmqHptpu8TfKU", 1245400 },
        {"SbEoHzETpFk1WuhSkNwbcYcARzCPQqu1VB", 1245400 },
        {"SQkis2s7aWJnLkj4yJNKVGCwCtHmZdrTur", 1245400 },
        {"SfN7BNUSaSASWmwtcpwb5dYgDvcvsJSBFD", 1245400 },
        {"SbGZhz7xZeGYiQMs2uQ5JKhxiWoQ7dsm6V", 1245400 },
        {"SaKtcx2RnFXU1HSEuDiPTLcupwtDsVEzK9", 1245400 },
        {"SSMMnrWDFNXCKv6UrBXvkNRN6Em4fwSr2w", 1245400 },
        {"SSLwkBJEcTvBAUP5Yxa6X5kRDwCHzDbCG8", 1245400 },
        {"Sc13SqPCXmga2Wzjx3aZDVwcJyz8Ju3jLg", 1245400 },
        {"Sd3Qzkxz8HjGToCZ748TiCKEKMtmZWHpZa", 1245400 },
        {"SWMPEJPiFZctUAiAX6qQFQqmdwcHPFfpQ6", 1245400 },
        {"SWmyRpD8wMJXVjkAxXbSAdHGaft4BJkC71", 1245400 },
        {"Saqn2HnFwAFdTe6zj8uMSe6cfYhbngHQ9F", 1245400 },
        {"SXihZ75qVJV8CBpZPhbxaFFnpfJXTrxQRW", 1245400 },
        {"SVjDiXabkvvVUSK6EWssxjroRZWkcYQmw5", 1245400 },
        {"Sc4hetMEEyxgXoBbGiAmzFNdcEJDptJX9w", 1245400 },
        {"SU6SELk4jcZu8aD3wHEYt4bSx1YZPQmC3d", 1245400 },
        {"SdvhzxBeiuCUupd97U3qPfPNX5maC54RMh", 1245400 },
        {"SZD6q4vgKXnwmwpewcoa6MMpCosTXi7Q87", 1245400 },
        {"STpGLXLWgnAVS3eAyc2AqTDRU4s4mPFQKJ", 1245400 },
        {"SW7EhRQsazTUnFhNsq1Q4JsCrotnE4QU2T", 1245400 },
        {"SS9VNKpbLjLoZBhmSQdj2NgDXdweo4CCfD", 1245400 },
        {"SRGgLQunsVz6yvYFBxuLxSEiryLpNv2CZg", 1245400 },
        {"Sdsay8ntkktCzwwpbZJAL5Tf6nMwtmGSrv", 1245400 },
        {"SkDG4v6zErJpPZWEKzG64FojspRhYLYFC6", 1245400 },
        {"SiLuee1FQJLRMJuJ7uZL6P1db1ATxP5JYc", 1245400 },
        {"SWGLdSmyTnVRY1xNTR7zg1KKv1ymVgSEHW", 1245400 },
        {"SdmZfw3QZKEUFGh3GBfpSvLCdYHQtdG7n6", 1245400 },
        {"SRuifzJD6Y51A6pMpNFfqQ4ZnHuE3cSz2f", 1245400 },
        {"Se9MjJ9pkQNpkANytHeFrvxQUuQFcofUUk", 1245400 },
        {"SjfsidssEkgPzQMd7Pg5yZ2WAwSX9vW2AQ", 1245400 },
        {"SY5mEK3s5kopwC9jSkdp2RN3BFVGfjMx2Q", 1245400 },
        {"SkB4gcxVb6sZBTh7igi2r1SDyzznQ5zqhd", 1245400 },
        {"ScXpxa4GwevJ4cYjkZdakNyTPGqxk7raor", 1245400 },
        {"Sgzk9AFkFuygSRxzYRegZzee3D3mgDyhoK", 1245400 },
        {"SUCnaHwtZ4TJhxySBsAG1yhF5SmCS8jahx", 1245400 },
        {"SSCJbcAcb3r4uiqSSKw1bU2H9Ys5Cr1pNt", 1245400 },
        {"SS3ec8FG3pjmMc488VhtAiR11XX18tWDmU", 1245400 },
        {"SSWVdyZKqQ9qr4ApkXjScyxVqX6nofAv9N", 1245400 },
        {"SbRmPyPiNKWMHbt6Kti7eRUqnxCiSu3bsP", 1245400 },
        {"SUXvjdp4MnSfjhPB4B7h1PYhcZbnyDPtrM", 1245400 },
        {"SaJvm59LprYD7vPBNfnkHadmPDgMdqnVdd", 1245400 },
        {"SfgHFeN3uMRXR1coNiodMtBJk1ZT6LNLMc", 1245400 },
        {"SVJaynNBQqRy1EZ3vx2AeYfu53iYqdhTjp", 1245400 },
        {"SbJa4MMoBqVZ3CN1aw3dzegjmgKjjbC9CF", 1245400 },
        {"SQ2hLV9o6Bbo1GFf828LaxS5q5dUXTV3MU", 1245400 },
        {"SYpJt8m8fan9E7GSjy1pLmRttFJ95JBS28", 1245400 },
        {"SiksrbNsxQDCp3TsqHRRQeAU99NDZCFbMq", 1245400 },
        {"STVxUdXitecDM5MmyrrzHxLguuPCFqktb4", 1245400 },
        {"SaQLrBVcnHnmQQE2WkgDrHbwah4iQ2X8r9", 1245400 },
        {"SS7CEtqZrhFV6qhMjo8PxLoipnNACaTtD2", 1245400 },
        {"SV54u6yhfrgzpoz1h73Aa6D3qMEAqzoVP8", 1245400 },
        {"SUiSccR7xSKKivMv9AHvLqQ8gi3Msgankr", 1245400 },
        {"SfN8Zoq4W3spi7CDgpBES1oeHZDt5kYf8a", 1245400 },
        {"Seab6P6GWcQbtmacZRU11z6xqGuTEujPXs", 1245400 },
        {"SXuJePRMqKXnxgCgghN2CbnZZQ9mPqR9XZ", 1245400 },
        {"SUcU47CUg94sqSjrASwY1yjfBvNbSjHDM6", 1245400 },
        {"SV7ojfZ4aqWT8ea8As1FPnS4nSg9xNPTYt", 1245400 },
        {"SYXmwoy7Twvb6aVDz9uJRke9veLMHnQM6z", 1245400 },
        {"SSkZyQqFMj1auUTSCSJxnoTZ2HDXNhXddD", 1245400 },
        {"Si8uyLQn2GeeVovKuphhe6zPUVEpdYi6rd", 1245400 },
        {"SVmkfGVorN4oDA5YwyE4rSKHSMhXCVRgFn", 1245400 },
        {"SUYrJwpdRs8xVBNtmD3k3E4gzTKB48Pa49", 1245400 },
        {"SZHun3QbXVgrJ9jPv6wYHrdXgJVxBteq6x", 1245400 },
        {"ScW9kuhJNFJsFC9qJraSnaVYvErfyKMY1E", 1245400 },
        {"SgpuS9uqzTXeSBBHG91aEY3JLUvnqQyh1q", 1245400 },
        {"SfCBBoV2kPFhJAVWFn3mB6zGpnvyJDUYWi", 1245400 },
        {"SfhQpFkXXChjMeyMLQpuopZy4PeZnoGG7c", 1245400 },
        {"SeupYXxYaPa3gsvkEdYeEnECMPSC6dWKHY", 1245400 },
        {"SZZKtR2rSz4GmZGqGSTDriHgkbCxB6CYF4", 1245400 },
        {"SRA9CyuGiXPQSWNQVFYsjDQ5XzrwbKDws8", 1245400 },
        {"Sj8BxGS8eto6jdj6pSVgZdrMTsAFqsmF9H", 1245400 },
        {"SZ26PxMUvK5oHKcbLYiv7cXyu1ReV6hsso", 1245400 },
        {"SSap88UfPE9CZHPwd8ErHd9TaBb1wMxkpa", 1245400 },
        {"SY23V4kiADVHq5N4fyfCY2TYcozbmCA4WN", 1245400 },
        {"SaQ6Wf5tzAxR1Xc5xtZofrLgR7u7DCE7Tp", 1245400 },
        {"Sjat7HGs27ww8sRyrhGDY6GTEbQouUJCXq", 1245400 },
        {"SVmFysvNi4eaakykr2NNmwQmV2x235hHgS", 1245400 },
        {"SaPMUi9Lwj2gzbH8sJkRd9V92hVFKzVfNU", 1245400 },
        {"Sif2esexP8rsnrvghRKfRqwTBQYBkFXwgt", 1245400 },
        {"Sc8T9nHfMiE8ey7MxvF43QwHiPcYhu39Le", 1245400 },
        {"Sg4TdofZgVPpx7JQVBkpqput8B6as64sQ5", 1245400 },
        {"SWcYmJVw5stE7CZbc5U1ULdQQGX8HMRfih", 1245400 },
        {"SbZTpUHMhbE2cH3siXdx4a6YaqVjdGvA7j", 1245400 },
        {"Sc9dLtGzDVWmPtEBE1socSRTqsLqR8zkLH", 1245400 },
        {"SWavi7bW5WedXxgxE3G5CBiJwkmgC4Bwnu", 1245400 },
        {"SY3AH68cg2L1BfjJWQF2eULupAfc5dqeD6", 1245400 },
        {"SiKPubPeKy8WA1FkmHdsoa639Twp8r6261", 1245400 },
        {"SRqho1FVAmF7ML5gU7Z7oR2eVJauKpzHdX", 1245400 },
        {"SabF66vzyV1P7kNckC8euefLtJNq5G7xG8", 1245400 },
        {"SfbgNruvVs621Y6LLLbV3vtwZSGmBuy3Mg", 1245400 },
        {"SUpHrAbJ4ERPNeNaLjVKaVVHr7fMtDTk7M", 1245400 },
        {"SQZkkhJgg7Lie2pvfjcGrFfv7xJ1GQbYeC", 1245400 },
        {"SgK87u64iMd69xJNPsBBg3egyXpdD8EoCC", 1245400 },
        {"ShGC6UqJy4Wt7v5eAgqhGrKLWNEAhVYnQT", 1245400 },
        {"SavNVk3PgQoMwgYokgqx35iER6DMfNK26u", 1245400 },
        {"SPkw3MPtTwC2GNEXZQtsXCqAPyLXdkpQBY", 1245400 },
        {"SZzAuLzFJBrJ2K8ANpta8QhMJzYSTJ2UYd", 1245400 },
        {"SdTBLdVVs1U2YYUbbmQ5WVjZm5CM5xccat", 1245400 },
        {"SegBhtaibZZRzBwbQ1GUVZWfVgKhYdkHg6", 1245400 },
        {"SVjHDsW3u88z1RsmCrHmdBmrLapxeQTB5Q", 1245400 },
        {"SguPHzg4EVXZSBSnv1iFeJAiGK6Gzr3iFo", 1245400 },
        {"Sa2BfhpgkuzYWqXQuwYY3Vxtf8QsGHNWQY", 1245400 },
        {"SUCmk7QKZkG2bM1XnxeNmHXFNgpumyCkaC", 1245400 },
        {"SWaRUqLEECDd18ZNi3SB8SjZemNabR9ESC", 1245400 },
        {"SbQqoJCLFipYjuEDVPQuQLeP1i2RZJpKCm", 1245400 },
        {"SdfmFN1bZC51yAvQxJoQqUaLZfKiS1CZBx", 1245400 },
        {"SPPPK49ULLahmipp1KFULjvbHicoykDyvf", 1245400 },
        {"SVu2psScBfX4HDXsDZTspBqc7QSEDtVoG8", 1245400 },
        {"Sde8aGeC8vK9qsDekFbaF3yC9fwPaZMs5u", 1245400 },
        {"STXWAt89RfcJtcPkqrYDi81BiH2iGLCSxA", 1245400 },
        {"SRsmCkwayAtjknrqA4r5nz2xkSyezDcSB4", 1245400 },
        {"SQLeuFqBGYJM86fLMLQhrzU4fCK2iiqQsz", 1245400 },
        {"SZ8y16pDGB2Uyie9MYTZzvSmuzGgrkmjed", 1245400 },
        {"SjZuvzxXRge4i5ygQGzB3gnCXPAstXU6H1", 1245400 },
        {"SUK87LK7wkNc2wXmVS557ydrPuSD7wvJJY", 1245400 },
        {"SWBEWq4aSVAAEXWV8DmX3CEd1THzAMKyNV", 1245400 },
        {"SgqgudtwAVoQmKe7pfnnKAEi4AzLZGTtoc", 1245400 },
        {"SjZE1hiirHomdP2aUGUA1szVQBRZ2cH3oG", 1245400 },
        {"SREd5zqHgS5x51mfCeVTpBZ9Ci22ZNfQbQ", 1245400 },
        {"Si1miUvcXfdXDERmiFVqnsbZPNk9eLcLzx", 1245400 },
        {"STDiAtpTtEzV6gVPTzeAiEFSGs5EnMAUs1", 1245400 },
        {"SZPEtzMzxA6ZS5nJyzaiVyJurUrhXbncEG", 1245400 },
        {"SgoiokKh3hqpAbYyFmPJBQaWbetJmfD3Pf", 1245400 },
        {"SV9hyPABdorjHSS84i8bAfoBXLeedc8ax2", 1245400 },
        {"SUFNG2WSLsCkJPfC3mcZubRqYpPbCChjoG", 1245400 },
        {"SghoXQxhA4JsszsebJYFpWRSaEoa1ZVHVn", 1245400 },
        {"SgLyFu2MFkbCKxieAWyJvWFU7TT3kyc5eK", 1245400 },
        {"SbPuAtorZbijp4tqHYjdLHBDLrzsmYcwKe", 1245400 },
        {"SR4VcSjBNq1UPdLqkPa2Z3sJ6SGSbhDaML", 1245400 },
        {"SW1s6TGcixZWcNA2fxMiKSfv6a43ue7Rrx", 1245400 },
        {"SebHuv6WGAFSEixDWjdCTjSD5YAMiCrFbA", 1245400 },
        {"ShYrfMpGUZyyakVHe2r6idjeL2gJqLZJed", 1245400 },
        {"SVW3oQu7yxDdpbF2T7DAJHsfkfMbceegWV", 1245400 },
        {"SW8VMN5R4yaDS5uR86uhB8oEriBqh97acy", 1245400 },
        {"SbiZYxGkcmrQDv4tuhadehKjW25VRLNqFP", 1245400 },
        {"SR6gY1SFuPtNkEvmUeF5iUXGxtGwNoZMGX", 1245400 },
        {"SWHgpKcvddG238e5sQzbqFa37uQHuznZtS", 1245400 },
        {"SWf5B8RtuVexcaMu8LxkHngCN2JBvakFDz", 1245400 },
        {"SQnmCBtkNioSdAy5HxZAPYFQQpfQCdYGPw", 1245400 },
        {"SWkGTDpeZkRMDLYQbzJTd657V7EekMmoiq", 1245400 },
        {"SUK3oD6Y2eLDjnKJdevpC4REmQRNYVwUu6", 1245400 },
        {"SRdxvpzLbxmepGS2TH6nqbjEn821YBUpqa", 1245400 },
        {"SRF3xWji8SGb5MMR57wDWCad5kf861YJRH", 1245400 },
        {"SiRsNT8BAAvsve96N8qYtJpb1LPgP3siYF", 1245400 },
        {"SURhhXtn9yM6iSo1YKZXHhmvxD78FysGPo", 1245400 },
        {"SWwDP6wRd9BYBfjTrxYrNNo6FFD9Wd1fuQ", 1245400 },
        {"SV8qavMKNERRfnqqK5XwgXQ4g5U2Aee7id", 1245400 },
        {"SZHZwmBZjcR3KnnCuEBCHsJLJwYpZRtSEd", 1245400 },
        {"Si5nrshFjEB6N6yTzvvZd3NWjNCZGAiwMw", 1245400 },
        {"SiDLVWSsrFKHTouFHF11Nd8bdU142TftJY", 1245400 },
        {"SV19oQswgiCVHozEMaXSYT7uGb1qA17R5f", 1245400 },
        {"SgvLQ1GC5Zj9YLMqBYoPRP2R84h93p728C", 1245400 },
        {"STMSgCdc6fUpQJMKTQ4xdQccuMEUK2F23k", 1245400 },
        {"SVtdoDTor6LKD7uS36uFva6zRjf2JnZ4y6", 1245400 },
        {"ScfinqKqMyTMDsZyuA6oMrVUvc58fKHfaU", 1245400 },
        {"STqYEsNzjJaor4Y9X26tNjBKz9NE1r2fNr", 1245400 },
        {"SQAkBFkG9tQwmBd9hvhrxgKdee9XSf18Ri", 1245400 },
        {"SSJsAWJFJSSrZbkKzBifL6PEZpJNjAtzL4", 1245400 },
        {"SbdLgxMnZYC1Lbp2aVM483tPBJv8SooHo6", 1245400 },
        {"SUDonXfKr7Y8opB1jgPPz4QN3ivWmTMTjh", 1245400 },
        {"Sk7Ew5FqkUJiLvGMXcneicC89x65c197vp", 1245400 },
        {"SPs2Cm5SREZssjdGRSniAe4jgdSQeafipw", 1245400 },
        {"SdYiHJ46PF3RVxn8UGcktfxLv8wc1FwWS3", 1245400 },
        {"SXztZogyniJ2TtoH7KijJjJHBQyqx9pinx", 1245400 },
        {"Sf5gpXm2DeXCNH2Aq9n9XbcjEX8YYFrwYj", 1245400 },
        {"SPVcLpKSRhxWZ8Z9CBWGekM1xBtrwUwxLQ", 1245400 },
        {"Sfcmp2egYJ41gbvy5g7TYm7863Jnh1bZxC", 1245400 },
        {"SazEQEGcReXSEne5d5uxZUV1nfy9daGcpU", 1245400 },
        {"SPLQhvofRqX5BmBpErARe2ZUrKcnCxLtT7", 1245400 },
        {"ScVtWaJhTiLZgUg1A143TaUfFwfGXm5S8T", 1245400 },
        {"STbJ98VbL1rrb2LES4ksNN5Jp4Q2ezvp84", 1245400 },
        {"SbKFW5rgocnbUGvkfq38cgf8LPKGJrrCxt", 1245400 },
        {"STfa4bKjBxizvuGKmaGkEtAd2t1k13obdW", 1245400 },
        {"SZcjWSzAXCzU8VYCxpgz9tTAgZoyDR3fDk", 1245400 },
        {"STDXKzZ2ejWu8qCjqmH7Ckc6hVhHz3VNoo", 1245400 },
        {"ShdXKA2zopB6DXJpxCoKTf3YHdiNCht5mM", 1245400 },
        {"Sgdvyd7G6Km7TeYHKxZ8F2f3nDtpWchnXz", 1245400 },
        {"SPZHoDDKovRNtAFiv99F6zWgWQMXVmuEDS", 1245400 },
        {"SUpL68uQ3yQDfxacMbnMW87mYRb5M8PYbR", 1245400 },
        {"SSXtsAQxVm8YYnT6G6KeY8niGxEhHqpm7v", 1245400 },
        {"Sfp9Rwsgogmw63YS6wGGiTNv46yTaDFSiC", 1245400 },
        {"SbkZj2PsE9aia8aoDzh2gtBh5XxgiaB2fS", 1245400 },
        {"SdixY3MeGzcLs4FJmapfYU1KWtgKyFPjCS", 1245400 }
        };

        // Network upgrades
        consensus.vUpgrades[Consensus::BASE_NETWORK].nActivationHeight                   = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight              = Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_POS].nActivationHeight                    = 371;
        consensus.vUpgrades[Consensus::UPGRADE_POS_V2].nActivationHeight                 = 1;
        consensus.vUpgrades[Consensus::UPGRADE_BIP65].nActivationHeight                  = 570;
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MODIFIER_V2].nActivationHeight      = 371;
        consensus.vUpgrades[Consensus::UPGRADE_TIME_PROTOCOL_V2].nActivationHeight       = 470;
        consensus.vUpgrades[Consensus::UPGRADE_P2PKH_BLOCK_SIGNATURES].nActivationHeight = 470;
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MIN_DEPTH_V2].nActivationHeight     = 900000;
        consensus.vUpgrades[Consensus::UPGRADE_MASTERNODE_RANK_V2].nActivationHeight     = 900000;

        consensus.vUpgrades[Consensus::UPGRADE_POS].hashActivationBlock                    = uint256S("0ed8d0e77ccb32c72ec2a650d1dad70cd084f0c06092d9bc2bc692dd93109ebf7");
        consensus.vUpgrades[Consensus::UPGRADE_POS_V2].hashActivationBlock                 = uint256S("000100bb93faebcafa31b6f1c1d398b69f542e1f5bf5e3063f0f7b5d91e1a460");
        consensus.vUpgrades[Consensus::UPGRADE_BIP65].hashActivationBlock                  = uint256S("bb38408518a354a2cde1a5af36e80fe01edbe0ba27098b6bc77ef10e3556cd63");
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MODIFIER_V2].hashActivationBlock      = uint256S("0ed8d0e77ccb32c72ec2a650d1dad70cd084f0c06092d9bc2bc692dd93109ebf7");
        consensus.vUpgrades[Consensus::UPGRADE_TIME_PROTOCOL_V2].hashActivationBlock       = uint256S("fb80a034eedc93dbe38a2874f683429860e2b3c8ea9974d6e69bf379296d9a6c");
        consensus.vUpgrades[Consensus::UPGRADE_P2PKH_BLOCK_SIGNATURES].hashActivationBlock = uint256S("fb80a034eedc93dbe38a2874f683429860e2b3c8ea9974d6e69bf379296d9a6c");
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MIN_DEPTH_V2].hashActivationBlock     = uint256S("c6fb0a9fb7e68a4b22a0d07f1b7304807f4217b8d60cf34469df94d4c229a5fa");
        consensus.vUpgrades[Consensus::UPGRADE_MASTERNODE_RANK_V2].hashActivationBlock     = uint256S("c6fb0a9fb7e68a4b22a0d07f1b7304807f4217b8d60cf34469df94d4c229a5fa");

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0x45;
        pchMessageStart[1] = 0x76;
        pchMessageStart[2] = 0x65;
        pchMessageStart[3] = 0xba;
        nDefaultPort = 62583;

        vSeeds.push_back(CDNSSeedData("seeder", "dnsseed.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed1", "seed1.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed2", "seed2.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed3", "seed3.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed4", "seed4.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed5", "seed5.safedeal.trade"));
        vSeeds.push_back(CDNSSeedData("seed6", "seed6.safedeal.trade"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 63);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 13);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 212);
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x02)(0x2D)(0x25)(0x33).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x02)(0x21)(0x31)(0x2B).convert_to_container<std::vector<unsigned char> >();
        // BIP44 coin type is from https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x77).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));
        //convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main)); // added
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }

};
static CMainParams mainParams;

/**
 * Testnet (v1)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";

        genesis = CreateGenesisBlock(1454124731, 2402015, 0x1e0ffff0, 1, 250 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        //assert(consensus.hashGenesisBlock == uint256S("0x0000041e482b9b9691d98eefb48473405c0b8ec31b76df3797c74a78680ef818"));
        //assert(genesis.hashMerkleRoot == uint256S("0x1b2ef6e2f28be914103a277377ae7729dcd125dfeb8bf97bd5964ba72b6dc39b"));

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.powLimit   = ~UINT256_ZERO >> 20;   // safedeal starting difficulty is 1 / 2^12
        consensus.posLimitV1 = ~UINT256_ZERO >> 24;
        consensus.posLimitV2 = ~UINT256_ZERO >> 20;
        consensus.nCoinbaseMaturity = 100;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMaxMoneyOut = 9999999999 * COIN;
        consensus.nPoolMaxTransactions = 3;
        consensus.nStakeMinAge = 60 * 60;
        consensus.nStakeMinDepth = 100;
        consensus.nStakeMinDepthV2 = 600;
        consensus.nTargetTimespan = 40 * 60;
        consensus.nTargetTimespanV2 = 30 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;

        // spork keys
        consensus.strSporkPubKey = "04E88BB455E2A04E65FCC41D88CD367E9CCE1F5A409BE94D8C2B4B35D223DED9C8E2F4E061349BA3A38839282508066B6DC4DB72DD432AC4067991E6BF20176127";
        consensus.strSporkPubKeyOld = "";
        consensus.nTime_EnforceNewSporkKey = 0;
        consensus.nTime_RejectOldSporkKey = 0;

        // Network upgrades
        consensus.vUpgrades[Consensus::BASE_NETWORK].nActivationHeight                   = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight              = Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_POS].nActivationHeight                    = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_POS_V2].nActivationHeight                 = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_BIP65].nActivationHeight                  = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MODIFIER_V2].nActivationHeight      = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TIME_PROTOCOL_V2].nActivationHeight       = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_P2PKH_BLOCK_SIGNATURES].nActivationHeight = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MIN_DEPTH_V2].nActivationHeight     = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_MASTERNODE_RANK_V2].nActivationHeight     = Consensus::NetworkUpgrade::ALWAYS_ACTIVE;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */

        pchMessageStart[0] = 0x45;
        pchMessageStart[1] = 0x76;
        pchMessageStart[2] = 0x65;
        pchMessageStart[3] = 0xba;
        nDefaultPort = 42972;

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.push_back(CDNSSeedData("tseeder", "tseeder.safedeal.trade", true));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 139); // Testnet safedeal addresses start with 'x' or 'y'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet safedeal script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet safedeal BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet safedeal BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet safedeal BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";

        genesis = CreateGenesisBlock(1454124731, 2402015, 0x1e0ffff0, 1, 250 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        //assert(consensus.hashGenesisBlock == uint256S("0x0000041e482b9b9691d98eefb48473405c0b8ec31b76df3797c74a78680ef818"));
        //assert(genesis.hashMerkleRoot == uint256S("0x1b2ef6e2f28be914103a277377ae7729dcd125dfeb8bf97bd5964ba72b6dc39b"));

        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.powLimit   = ~UINT256_ZERO >> 20;   // safedeal starting difficulty is 1 / 2^12
        consensus.posLimitV1 = ~UINT256_ZERO >> 24;
        consensus.posLimitV2 = ~UINT256_ZERO >> 20;
        consensus.nCoinbaseMaturity = 100;
        consensus.nFutureTimeDriftPoW = 7200;
        consensus.nFutureTimeDriftPoS = 180;
        consensus.nMaxMoneyOut = 43199500 * COIN;
        consensus.nPoolMaxTransactions = 2;
        consensus.nStakeMinAge = 0;
        consensus.nStakeMinDepth = 2;
        consensus.nTargetTimespan = 40 * 60;
        consensus.nTargetTimespanV2 = 30 * 60;
        consensus.nTargetSpacing = 1 * 60;
        consensus.nTimeSlotLength = 15;

        /* Spork Key for RegTest:
        WIF private key: 932HEevBSujW2ud7RfB1YF91AFygbBRQj3de3LyaCRqNzKKgWXi
        private key hex: bd4960dcbd9e7f2223f24e7164ecb6f1fe96fc3a416f5d3a830ba5720c84b8ca
        Address: yCvUVd72w7xpimf981m114FSFbmAmne7j9
        */
        consensus.strSporkPubKey = "043969b1b0e6f327de37f297a015d37e2235eaaeeb3933deecd8162c075cee0207b13537618bde640879606001a8136091c62ec272dd0133424a178704e6e75bb7";
        consensus.strSporkPubKeyOld = "";
        consensus.nTime_EnforceNewSporkKey = 0;
        consensus.nTime_RejectOldSporkKey = 0;

        // Network upgrades
        consensus.vUpgrades[Consensus::BASE_NETWORK].nActivationHeight =
                Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
                Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_POS].nActivationHeight           = 251;
        consensus.vUpgrades[Consensus::UPGRADE_POS_V2].nActivationHeight        = 251;
        consensus.vUpgrades[Consensus::UPGRADE_BIP65].nActivationHeight         =
                Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_STAKE_MODIFIER_V2].nActivationHeight          = 251;
        consensus.vUpgrades[Consensus::UPGRADE_TIME_PROTOCOL_V2].nActivationHeight          =
                Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_P2PKH_BLOCK_SIGNATURES].nActivationHeight       = 300;

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */

        pchMessageStart[0] = 0xa1;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xac;
        nDefaultPort = 52972;

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }

    void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
    {
        assert(idx > Consensus::BASE_NETWORK && idx < Consensus::MAX_NETWORK_UPGRADES);
        consensus.vUpgrades[idx].nActivationHeight = nActivationHeight;
    }
};
static CRegTestParams regTestParams;

static CChainParams* pCurrentParams = 0;

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}

void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
{
    regTestParams.UpdateNetworkUpgradeParameters(idx, nActivationHeight);
}
