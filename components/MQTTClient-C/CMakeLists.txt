#*******************************************************************************
#  Copyright (c) 2017 IBM Corp.
#
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution.
#
#  The Eclipse Public License is available at
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at
#    http://www.eclipse.org/org/documents/edl-v10.php.
#
#  Contributors:
#     Ian Craggs - initial version
#*******************************************************************************/

# project("paho-mqttclient" C)

# ADD_SUBDIRECTORY(src)
# ADD_SUBDIRECTORY(samples)
# ADD_SUBDIRECTORY(test)

set(COMPONENT_REQUIRES MQTTPacket sim800l mbedtls mbedtls_sim800)
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src/MQTTClient.c" "src/FreeRTOS/MQTTFreeRTOS.c")
set(COMPONENT_ADD_INCLUDEDIRS "src/" "src/FreeRTOS/")

register_component()