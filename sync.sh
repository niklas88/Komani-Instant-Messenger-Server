#!/bin/bash
rsync -r -avz --exclude=*.d --exclude=*.o --exclude=*.a -e ssh . niklas@sceneproject.org:kimd/
