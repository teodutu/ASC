import traceback
import sys, os, subprocess, signal
from subprocess import STDOUT, check_output

programs = [
    #'std_hashtable',
    'gpu_hashtable',
]

testConfig = [
    {   "testName" : "T1",
        "numEntries" : 1000000,
        "numChunks" : 1,
        "tags" : {
            "HASH_BATCH_INSERT" : { "minThroughput" : 5, "minLoadFactor" : 60, "points" : 10 },
            "HASH_BATCH_GET" : { "minThroughput" : 10, "minLoadFactor" : 60, "points" : 10 },
        }
    },
    {   "testName" : "T2",
        "numEntries" : 2000000,
        "numChunks" : 1,
        "tags" : {
            "HASH_BATCH_INSERT" : { "minThroughput" : 5, "minLoadFactor" : 70, "points" : 5 },
            "HASH_BATCH_GET" : { "minThroughput" : 25, "minLoadFactor" : 70, "points" : 5 },
        }
    },
    {   "testName" : "T3",
        "numEntries" : 4000000,
        "numChunks" : 2,
        "tags" : {
            "HASH_BATCH_INSERT" : {"minThroughput" : 5, "minLoadFactor" : 75, "points" : 5 },
            "HASH_BATCH_GET" : { "minThroughput" : 25, "minLoadFactor" : 75, "points" : 5 },
        }
    },
    {   "testName" : "T4",
        "numEntries" : 10000000,
        "numChunks" : 4,
        "tags" : {
            "HASH_BATCH_INSERT" : { "minThroughput" : 2, "minLoadFactor" : 80, "points" : 5 },
            "HASH_BATCH_GET" : { "minThroughput" : 20, "minLoadFactor" : 80, "points" : 5 },
        }
     },
]

outfile = open('output', 'w')

if len(sys.argv) > 1:
    benchtypes = sys.argv[1:]
else:
    benchtypes = ('0', '1')

for program in programs:

    programName = program
    hwMaxPoints = 90
    hwPoints = 0

    for testEntry in testConfig:
        
        try:
            output = subprocess.check_output(['./' + program, str(testEntry["numEntries"]), str(testEntry["numChunks"])])
            lines = str(output).split("\n")
            
            testPoints = 0
            testName = testEntry["testName"]
            
            print( "-------------- Test %s --------------" % (testName) )
            
            for line in lines:

                words = str(line).split(",")
                tagName = words[0]

                if tagName == "ERR":

                    testPoints = 0
                    print(line, " INVALIDATED")
                    break

                if tagName == "HASH_BATCH_INSERT" or tagName == "HASH_BATCH_GET":
                    
                    hashThroughput = float(words[2])
                    loadFactor = float(words[3])
                    testReq = testEntry["tags"][tagName]

                    if (hashThroughput >= testReq["minThroughput"]) and (loadFactor >= testReq["minLoadFactor"]):

                        testPoints = testPoints + testReq["points"]
                        print( "OK\t +%d pts\t %s" % (testReq["points"], line) )
                        
                    else:

                        print( "FAILED\t   0 pts\t %s\t REQUIREMENTS %s" % (line, testReq) )
                        
            hwPoints = hwPoints + testPoints
            print( "TOTAL\t +%d pts\n" % (testPoints) )
 
        except Exception:

            traceback.print_exc()
            print( "Error with %s" % str(['./' + program, str(testEntry["numEntries"]), str(testEntry["numChunks"])]))
            break

    print( "\nTOTAL %s  %d/%d" % (program, hwPoints, hwMaxPoints) )
        
