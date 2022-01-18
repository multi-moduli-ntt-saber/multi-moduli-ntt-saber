#!/usr/bin/env python3

import datetime
import subprocess
import sys

import serial
import numpy as np
from config import Settings

benchType = "f_speed"
outFileName = "f_speed.txt"
iterations = 100
testedList = [["MatrixVectorMul_A", "MatrixVectorMul_A cycles:"],
              ["MatrixVectorMul_D", "MatrixVectorMul_D cycles:"],
              ["InnerProd (Encrypt)", "InnerProd (Encrypt) cycles:"],
              ["InnerProd (Decrypt)", "InnerProd (Decrypt) cycles:"],
              ["InnerProd stack", "InnerProd_stack cycles:"],
              ["32-bit NTT", "32-bit NTT forward cycles:"],
              ["16-bit NTT", "16-bit NTT forward cycles:"],
              ["16-bit NTT light", "16-bit NTT forward light cycles:"],
              ["32-bit iNTT", "32-bit NTT inverse cycles:"],
              ["32-bit base_mul", "32-bit base_mul cycles:"],
              ["32x16-bit base_mul", "32x16-bit base_mul cycles:"],
              ["16-bit base_mul", "16-bit base_mul cycles:"],
              ["16x16 CRT", "16x16 CRT cycles:"],
              ["One mod", "One mod cycles:"]
             ]
schemeList = ["lightsaber", "saber", "firesaber"]
impleList = ["speed", "stack"]
cpu = "m4f"


def toLog(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000)}k"
  else:
    value = f"{value}"
  return f"{name}: {value}\n"

def getBinary(scheme, impl):
    return f"bin/crypto_kem_{scheme}_{impl}_{benchType}.bin"

def getFlash(binary):
    return f"st-flash write {binary} 0x8000000"

def run_bench(scheme, impl):
    binary = getBinary(scheme, impl)

    try:
        subprocess.check_call(getFlash(binary), shell=True)
    except:
        print("st-flash failed --> retry")
        return run_bench(scheme, impl)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, Settings.BAUD_RATE, timeout=10) as dev:
        logs = []
        iteration = 0
        log = b""
        while iteration < iterations:
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_bench(scheme, impl)
            sys.stdout.buffer.write(device_output)
            sys.stdout.flush()
            log += device_output
            if device_output == b'#':
                logs.append(log)
                log = b""
                iteration += 1
    return logs


def parseLogSpeed(log, ignoreErrors):
    log = log.decode(errors="ignore")
    if "error" in log.lower() and not ignoreErrors:
        raise Exception("error in scheme. this is very bad.")
    lines = str(log).splitlines()

    def get(lines, key):
        if key in lines:
            return int(lines[1+lines.index(key)])
        else:
            return None

    def cleanNullTerms(d):
        return {
            k:v
            for k, v in d.items()
            if v is not None
        }

    return cleanNullTerms({
            k: get(lines, v)
            for k, v in testedList
    })

def average(results):
    avgs = dict()
    for key in results[0].keys():
        avgs[key] = int(np.array([results[i][key] for i in range(len(results))]).mean())
    return avgs


def bench(scheme, texName, impl, outfile, ignoreErrors=False):
    logs    = run_bench(scheme, impl)
    results = []
    for log in logs:
        try:
            result = parseLogSpeed(log, ignoreErrors)
        except:
            breakpoint()
            print("parsing log failed -> retry")
            return bench(scheme, texName, impl, outfile)
        results.append(result)

    avgResults = average(results)
    print(f"{cpu} results for {scheme} (impl={impl})", file=outfile)

    for key, value in avgResults.items():
        macro = toLog(f"{texName}{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)

def makeAll():
    subprocess.check_call(f"make clean", shell=True)
    subprocess.check_call(f"make -j4 ITERATIONS={iterations}", shell=True)

with open(outFileName, "w") as outfile:

    now = datetime.datetime.now(datetime.timezone.utc)
    print(f"{benchType} measurements written on {now}; iterations={iterations}\n", file=outfile)

    makeAll()

    for scheme in schemeList:
        for imple in impleList:
            bench(scheme, scheme + cpu + imple, cpu + imple, outfile)