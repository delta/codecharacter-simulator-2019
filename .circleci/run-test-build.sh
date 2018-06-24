#!/usr/bin/env bash
curl --user ${USER_TOKEN}: \
    --request POST \
    --form revision="$(git log | head -n 1 | awk '{print $2}')"\
    --form config=@config.yml \
    --form notify=false \
    https://circleci.com/api/v1.1/project/github/delta/codecharacter-simulator-2019/tree/$(git branch | head -n 1 | awk '{print $2}')
