#!/usr/bin/env python3

import random

alphabet = "12345abcdefghijklmnoprstuvwxyz"

print(''.join(random.choices(alphabet, weights=None, k=12)))
