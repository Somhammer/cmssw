import FWCore.ParameterSet.Config as cms

from math import pi
from DQMServices.Core.DQMEDAnalyzer import DQMEDAnalyzer
rpcRecHitsDQM = DQMEDAnalyzer('RPCMonitorRecHits',
    rpcRecHits = cms.InputTag("rpcRecHits"),
    subFolder = cms.untracked.string("AllHits"),

    fillRollCenter = cms.untracked.bool(True),
    nbinsPhi = cms.untracked.double(100),

    setAxisLabel = cms.untracked.bool(True),
    binLabelsR = cms.PSet(
        RE12 = cms.untracked.vstring("", "A", "Ring1 B", "C", "", "A", "Ring2 B", "C", ""),
        RE34 = cms.untracked.vstring("", "A", "Ring1 B", "C", "", "A", "Ring2 B", "C", ""),
    ),
    binLabelsZ = cms.PSet(
        RB = cms.untracked.vstring([x for x in " BF BF BF BF BF "]),
        RB2out = cms.untracked.vstring([x for x in " BMF BF BF BF BMF "]),
        RB2in = cms.untracked.vstring([x for x in " BF BMF BMF BMF BF "]),
    ),
    binLabelsPhi = cms.PSet(
        RB = cms.untracked.vstring(
            "", "S01", "", "S02", "", "S03", "", "S04", "", "S05", "", "S06",
            "", "S07", "", "S08", "", "S09", "", "S10", "", "S11", "", "S12"
        ),
        RB3 = cms.untracked.vstring(
            "", "S01-","S01+", "", "S02-","S02+", "", "S03-","S03+", "", "S04-","S04+", "", "S05-","S05+", "", "S06-","S06+",
            "", "S07-","S07+", "", "S08-","S08+", "", "S09-","S09+", "", "S10-","S10+", "", "S11-","S11+", "", "S12-","S12+",
        ),
        RB4 = cms.untracked.vstring(
            "", "S01-","S01+", "", "S02-","S02+", "", "S03-","S03+",
            "", "S04--","S04-","","S04+","S04++",
            "", "S05-","S05+", "", "S06-","S06+", "", "S07-","S07+", "", "S08-","S08+",
            "", "S09",
            "", "S10-","S10+",
            "", "S11",
            "", "S12-","S12+",
        ),
    ),
    binEdgesZ = cms.PSet(
        RB1in  = cms.untracked.vdouble(-700, -655,-530,-410, -390,-264,-145, -120,5,120, 145,264,390, 410,530,655, 700),
        RB1out = cms.untracked.vdouble(-700, -655,-530,-410, -390,-264,-145, -120,5,120, 145,264,390, 410,530,655, 700),
        RB2in  = cms.untracked.vdouble(-700, -655,-530,-410, -390,-305,-225,-145, -120,-35,35,120, 145,225,305,390, 410,530,655, 700),
        RB2out = cms.untracked.vdouble(-700, -655,-570,-490,-410, -390,-265,-145, -120,5,120, 145,265,390, 410,490,570,655, 700),
        RB3    = cms.untracked.vdouble(-700, -655,-530,-410, -390,-265,-145, -120,5,120, 145,265,390, 410,530,655, 700),
        RB4    = cms.untracked.vdouble(-700, -655,-530,-410, -390,-265,-145, -120,5,120, 145,265,390, 410,530,655, 700),
    ),
    binEdgesR = cms.PSet(
        RE1 = cms.untracked.vdouble(0, 278,358,408,463, 509,565,643,684, 800),
        RE2 = cms.untracked.vdouble(0, 333,381,436,499, 505,566,643,696, 800),
        RE3 = cms.untracked.vdouble(0, 333,381,436,499, 505,566,643,696, 800), # There can be RE3/1 and RE4/1 in the upgrade
        RE4 = cms.untracked.vdouble(0, 333,381,436,499, 505,566,643,696, 800), # There can be RE3/1 and RE4/1 in the upgrade
    ),
    binEdgesPhi = cms.PSet(
        RB1in = cms.untracked.vdouble(
           -0.4,
           -0.158,0.328,0.367,   0.851,0.890,1.375,   1.414,1.898,1.938,   2.422,2.461,2.946,
            2.985,3.469,3.508,   3.993,4.032,4.516,   4.555,5.040,5.079,   5.563,5.603,6.087,
            2*pi
        ),
        RB1out = cms.untracked.vdouble(
           -0.4,
           -0.155,0.295,0.369,   0.818,0.893,1.342,   1.416,1.865,1.940,   2.389,2.463,2.913,
            2.987,3.436,3.510,   3.960,4.034,4.483,   4.558,5.007,5.081,   5.530,5.605,6.054,
            2*pi
        ),
        RB2in = cms.untracked.vdouble(
           -0.4,
           -0.279,0.211,0.244,   0.735,0.768,1.258,   1.291,1.782,1.815,   2.305,2.339,2.829,
            2.862,3.352,3.386,   3.876,3.909,4.400,   4.433,4.923,4.956,   5.447,5.480,5.970,
            2*pi-0.279,2*pi
        ),
        RB2out = cms.untracked.vdouble(
           -0.4,
           -0.263,0.196,0.261,   0.720,0.784,1.243,   1.308,1.767,1.831,   2.291,2.355,2.814,
            2.879,3.338,3.402,   3.861,3.926,4.385,   4.449,4.908,4.973,   5.432,5.496,5.956,
            2*pi
        ),
        RB3 = cms.untracked.vdouble(
           -0.4, ## Padding bottom
           -0.215,0.030,0.270,   0.310,0.555,0.795,   0.835,1.080,1.320,   1.360,1.600,1.840,
            1.880,2.125,2.365,   2.405,2.650,2.890,   2.930,3.170,3.415,   3.450,3.695,3.950,
            3.975,4.215,4.460,   4.500,4.740,4.985,   5.020,5.265,5.520,   5.545,5.790,6.030,
            2*pi ## Padding top
        ),
        RB4 = cms.untracked.vdouble(
           -0.4, ## Padding bottom
           -0.390,-0.130, 0.130,   0.150,0.395,0.655,   0.675,0.920,1.160, ## S01-+, S02-+, S03-+
            1.200,1.350,1.555, 1.600,1.790,1.950, ## S04--,-/+,++
            2.000,2.220,2.465,   2.486,2.747,2.990,   3.010,3.270,3.515,   3.535,3.790,4.000, ## S05-+,S06-+,S07-+S08-+
            4.060,4.335, ## S09
            4.360,4.700,5.060,  ## S10+-
            5.090,5.370, ## S11
            5.430,5.630,5.900, ## S12
            2*pi ## Padding top
        ),

        RE = cms.untracked.vdouble(
            -3.054, -2.88 , -2.705, -2.531, -2.356, -2.182,
            -2.007, -1.833, -1.658, -1.484, -1.309, -1.134,
            -0.96 , -0.785, -0.611, -0.436, -0.262, -0.087,
             0.087,  0.262,  0.436,  0.611,  0.785,  0.96 ,
             1.134,  1.309,  1.484,  1.658,  1.833,  2.007,
             2.182,  2.356,  2.531,  2.705,  2.88 ,  3.054, 2*pi-3.054
        ),
    ),
)

