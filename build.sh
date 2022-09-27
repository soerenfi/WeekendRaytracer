#!/bin/bash

export VULKAN_SDK=/opt/vulkan/x86_64

premake5 gmake2 && make config=release