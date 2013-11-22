/*******************************************************************************

 Eurecom OpenAirInterface 2
 Copyright(c) 1999 - 2010 Eurecom

 This program is free software; you can redistribute it and/or modify it
 under the terms and conditions of the GNU General Public License,
 version 2, as published by the Free Software Foundation.

 This program is distributed in the hope it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 more details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

 The full GNU General Public License is included in this distribution in
 the file called "COPYING".

 Contact Information
 Openair Admin: openair_admin@eurecom.fr
 Openair Tech : openair_tech@eurecom.fr
 Forums       : http://forums.eurecom.fsr/openairinterface
 Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

 *******************************************************************************/

/*! \file rrc_eNB_S1AP.c
 * \brief rrc S1AP procedures for eNB
 * \author Laurent Winckel
 * \date 2013
 * \version 1.0
 * \company Eurecom
 * \email: laurent.winckel@eurecom.fr and navid.nikaein@eurecom.fr
 */

#if defined(ENABLE_USE_MME)
# include "defs.h"
# include "extern.h"
# include "RRC/L2_INTERFACE/openair_rrc_L2_interface.h"
# include "RRC/LITE/MESSAGES/asn1_msg.h"
# include "rrc_eNB_S1AP.h"

# if defined(ENABLE_ITTI)
#   include "asn1_conversions.h"
#   include "intertask_interface.h"
#   include "pdcp.h"
#   include "s1ap_eNB.h"
# else
#   include "../../S1AP/s1ap_eNB.h"
# endif

/* Value to indicate an invalid UE initial id */
static const uint16_t UE_INITIAL_ID_INVALID = 0;

# if defined(ENABLE_ITTI)
/*! \fn uint16_t get_next_ue_initial_id(uint8_t mod_id)
 *\brief provide an UE initial ID for S1AP initial communication.
 *\param mod_id Instance ID of eNB.
 *\return the UE initial ID.
 */
static uint16_t get_next_ue_initial_id(uint8_t mod_id) {
  static uint16_t ue_initial_id[NUMBER_OF_eNB_MAX];

  ue_initial_id[mod_id]++;

  /* Never use UE_INITIAL_ID_INVALID this is the invalid id! */
  if (ue_initial_id[mod_id] == UE_INITIAL_ID_INVALID) {
    ue_initial_id[mod_id]++;
  }

  return ue_initial_id[mod_id];
}

/*! \fn uint8_t get_UE_index_from_initial_id (uint8_t mod_id, uint16_t ue_initial_id)
 *\brief retrieve UE index in the eNB from the UE initial ID.
 *\param mod_id Instance ID of eNB.
 *\param ue_initial_id The UE initial ID sent to S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_initial_id(uint8_t mod_id, uint16_t ue_initial_id) {
  uint8_t ue_index;
  static const uint8_t null_identity[5] =
    {0, 0, 0, 0, 0};

  DevCheck(mod_id < NB_eNB_INST, mod_id, NB_eNB_INST, 0);

  for (ue_index = 0; ue_index < NUMBER_OF_UE_MAX; ue_index++) {
    /* Check if this UE is in use */
    if (memcmp (eNB_rrc_inst[mod_id].Info.UE_list[ue_index], null_identity,
                sizeof(eNB_rrc_inst[0].Info.UE_list[ue_index])) != 0) {
      /* Check if the initial id match */
      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id == ue_initial_id) {
        return ue_index;
      }
    }
  }
  return UE_INDEX_INVALID;
}

/*! \fn uint8_t get_UE_index_from_eNB_ue_s1ap_id(uint8_t mod_id, uint16_t eNB_ue_s1ap_id)
 *\brief retrieve UE index in the eNB from the eNB_ue_s1ap_id previously transmitted by S1AP.
 *\param mod_id Instance ID of eNB.
 *\param eNB_ue_s1ap_id The value sent by S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_eNB_ue_s1ap_id(uint8_t mod_id, uint16_t eNB_ue_s1ap_id) {
  uint8_t ue_index;
  static const uint8_t null_identity[5] =
    {0, 0, 0, 0, 0};

  DevCheck(mod_id < NB_eNB_INST, mod_id, NB_eNB_INST, 0);

  for (ue_index = 0; ue_index < NUMBER_OF_UE_MAX; ue_index++) {
    /* Check if this UE is in use */
    if (memcmp (eNB_rrc_inst[mod_id].Info.UE_list[ue_index], null_identity,
                sizeof(eNB_rrc_inst[0].Info.UE_list[ue_index])) != 0) {
      /* Check if the initial id match */
      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id == eNB_ue_s1ap_id) {
        return ue_index;
      }
    }
  }
  return UE_INDEX_INVALID;
}

/*! \fn uint8_t get_UE_index_from_s1ap_ids(uint8_t mod_id, uint16_t ue_initial_id, uint16_t eNB_ue_s1ap_id)
 *\brief retrieve UE index in the eNB from the UE initial ID if not equal to UE_INDEX_INVALID or
 *\brief from the eNB_ue_s1ap_id previously transmitted by S1AP.
 *\param mod_id Instance ID of eNB.
 *\param ue_initial_id The UE initial ID sent to S1AP.
 *\param eNB_ue_s1ap_id The value sent by S1AP.
 *\return the UE index or UE_INDEX_INVALID if not found.
 */
static uint8_t get_UE_index_from_s1ap_ids(uint8_t mod_id, uint16_t ue_initial_id, uint16_t eNB_ue_s1ap_id) {
  uint8_t ue_index;

  if (ue_initial_id == UE_INITIAL_ID_INVALID) {
    /* If "ue_initial_id" is not set search if "eNB_ue_s1ap_id" is know by RRC */
    ue_index = get_UE_index_from_eNB_ue_s1ap_id (mod_id, eNB_ue_s1ap_id);
  }
  else {
    /* If "ue_initial_id" is set there is probably not yet an associated "eNB_ue_s1ap_id" with S1AP */
    ue_index = get_UE_index_from_initial_id (mod_id, ue_initial_id);
  }

  return ue_index;
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_INITIAL_CONTEXT_SETUP_RESP(uint8_t mod_id, uint8_t ue_index) {
  eNB_RRC_UE_INFO *UE_info = &eNB_rrc_inst[mod_id].Info.UE[ue_index];
  MessageDef *msg_p;
  int e_rab;
  int e_rabs_done = 0;
  int e_rabs_failed = 0;

  msg_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_INITIAL_CONTEXT_SETUP_RESP);
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).eNB_ue_s1ap_id = UE_info->eNB_ue_s1ap_id;
  for (e_rab = 0; e_rab < UE_info->index_of_e_rabs; e_rab++) {
    if (UE_info->e_rab[e_rab].status == E_RAB_STATUS_DONE) {
      e_rabs_done++;
      S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs[e_rab].e_rab_id = UE_info->e_rab[e_rab].param.e_rab_id;
      // TODO add other information from S1-U when it will be integrated
    }
    else {
      e_rabs_failed++;
      S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).e_rabs_failed[e_rab].e_rab_id = UE_info->e_rab[e_rab].param.e_rab_id;
      // TODO add cause when it will be integrated
    }
  }
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).nb_of_e_rabs = e_rabs_done;
  S1AP_INITIAL_CONTEXT_SETUP_RESP (msg_p).nb_of_e_rabs_failed = e_rabs_failed;

  itti_send_msg_to_task (TASK_S1AP, mod_id, msg_p);
}
# endif

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_UPLINK_NAS(uint8_t mod_id, uint8_t ue_index, UL_DCCH_Message_t *ul_dcch_msg) {
#if defined(ENABLE_ITTI)
  {
    ULInformationTransfer_t *ulInformationTransfer = &ul_dcch_msg->message.choice.c1.choice.ulInformationTransfer;

    if ((ulInformationTransfer->criticalExtensions.present == ULInformationTransfer__criticalExtensions_PR_c1)
        && (ulInformationTransfer->criticalExtensions.choice.c1.present
            == ULInformationTransfer__criticalExtensions__c1_PR_ulInformationTransfer_r8)
        && (ulInformationTransfer->criticalExtensions.choice.c1.choice.ulInformationTransfer_r8.dedicatedInfoType.present
            == ULInformationTransfer_r8_IEs__dedicatedInfoType_PR_dedicatedInfoNAS)) {
      /* This message hold a dedicated info NAS payload, forward it to NAS */
      struct ULInformationTransfer_r8_IEs__dedicatedInfoType *dedicatedInfoType =
          &ulInformationTransfer->criticalExtensions.choice.c1.choice.ulInformationTransfer_r8.dedicatedInfoType;
      uint32_t pdu_length;
      uint8_t *pdu_buffer;
      MessageDef *msg_p;

      pdu_length = dedicatedInfoType->choice.dedicatedInfoNAS.size;
      pdu_buffer = dedicatedInfoType->choice.dedicatedInfoNAS.buf;

      msg_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_UPLINK_NAS);
      S1AP_UPLINK_NAS (msg_p).eNB_ue_s1ap_id = eNB_rrc_inst[mod_id].Info.UE[ue_index].eNB_ue_s1ap_id;
      S1AP_UPLINK_NAS (msg_p).nas_pdu.length = pdu_length;
      S1AP_UPLINK_NAS (msg_p).nas_pdu.buffer = pdu_buffer;

      itti_send_msg_to_task (TASK_S1AP, mod_id, msg_p);
    }
  }
#else
  {
    ULInformationTransfer_t *ulInformationTransfer;
    ulInformationTransfer =
    &ul_dcch_msg->message.choice.c1.choice.
    ulInformationTransfer;

    if (ulInformationTransfer->criticalExtensions.present ==
        ULInformationTransfer__criticalExtensions_PR_c1)
    {
      if (ulInformationTransfer->criticalExtensions.choice.c1.present ==
          ULInformationTransfer__criticalExtensions__c1_PR_ulInformationTransfer_r8)
      {

        ULInformationTransfer_r8_IEs_t
        *ulInformationTransferR8;
        ulInformationTransferR8 =
        &ulInformationTransfer->criticalExtensions.choice.
        c1.choice.ulInformationTransfer_r8;
        if (ulInformationTransferR8->dedicatedInfoType.
            present ==
            ULInformationTransfer_r8_IEs__dedicatedInfoType_PR_dedicatedInfoNAS)
        s1ap_eNB_new_data_request (mod_id, ue_index,
            ulInformationTransferR8->
            dedicatedInfoType.choice.
            dedicatedInfoNAS.buf,
            ulInformationTransferR8->
            dedicatedInfoType.choice.
            dedicatedInfoNAS.size);
      }
    }
  }
#endif
}

/*------------------------------------------------------------------------------*/
void rrc_eNB_send_S1AP_NAS_FIRST_REQ(uint8_t mod_id, uint8_t ue_index,
                                     RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete) {
#if defined(ENABLE_ITTI)
  {
    MessageDef *message_p;

    message_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_NAS_FIRST_REQ);
    eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id = get_next_ue_initial_id (mod_id);
    S1AP_NAS_FIRST_REQ (message_p).ue_initial_id = eNB_rrc_inst[mod_id].Info.UE[ue_index].ue_initial_id;

    /* Assume that cause is coded in the same way in RRC and S1ap, just check that the value is in S1ap range */
    DevCheck(eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause < RRC_CAUSE_LAST,
             eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause, RRC_CAUSE_LAST, mod_id);
    S1AP_NAS_FIRST_REQ (message_p).establishment_cause = eNB_rrc_inst[mod_id].Info.UE[ue_index].establishment_cause;

    /* Forward NAS message */S1AP_NAS_FIRST_REQ (message_p).nas_pdu.buffer =
        rrcConnectionSetupComplete->dedicatedInfoNAS.buf;
    S1AP_NAS_FIRST_REQ (message_p).nas_pdu.length = rrcConnectionSetupComplete->dedicatedInfoNAS.size;

    /* Fill UE identities with available information */
    {
      S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask = UE_IDENTITIES_NONE;

      if (eNB_rrc_inst[mod_id].Info.UE[ue_index].Initialue_identity_s_TMSI.presence) {
        /* Fill s-TMSI */
        UE_S_TMSI *s_TMSI = &eNB_rrc_inst[mod_id].Info.UE[ue_index].Initialue_identity_s_TMSI;

        S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask |= UE_IDENTITIES_s_tmsi;
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.mme_code = s_TMSI->mme_code;
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.s_tmsi.m_tmsi = s_TMSI->m_tmsi;
      }

      if (rrcConnectionSetupComplete->registeredMME != NULL) {
        /* Fill GUMMEI */
        struct RegisteredMME *r_mme = rrcConnectionSetupComplete->registeredMME;

        S1AP_NAS_FIRST_REQ (message_p).ue_identity.presenceMask |= UE_IDENTITIES_gummei;
        if (r_mme->plmn_Identity != NULL) {
          if ((r_mme->plmn_Identity->mcc != NULL) && (r_mme->plmn_Identity->mcc->list.count > 0)) {
            /* Use first indicated PLMN MCC if it is defined */
            S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mcc = *r_mme->plmn_Identity->mcc->list.array[0];
          }
          if (r_mme->plmn_Identity->mnc.list.count > 0) {
            /* Use first indicated PLMN MNC if it is defined */
            S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mnc = *r_mme->plmn_Identity->mnc.list.array[0];
          }
        }
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_code = BIT_STRING_to_uint8 (&r_mme->mmec);
        S1AP_NAS_FIRST_REQ (message_p).ue_identity.gummei.mme_group_id = BIT_STRING_to_uint16 (&r_mme->mmegi);
      }
    }

    itti_send_msg_to_task (TASK_S1AP, mod_id, message_p);
  }
#else
  {
    s1ap_eNB_new_data_request (mod_id, ue_index,
        rrcConnectionSetupComplete->dedicatedInfoNAS.
        buf,
        rrcConnectionSetupComplete->dedicatedInfoNAS.
        size);
  }
#endif
}

# if defined(ENABLE_ITTI)
/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_DOWNLINK_NAS(MessageDef *msg_p, const char *msg_name, instance_t instance, mui_t *rrc_eNB_mui) {
  uint8_t ue_index;
  uint32_t length;
  uint8_t *buffer;

  ue_index = get_UE_index_from_s1ap_ids (instance, S1AP_DOWNLINK_NAS (msg_p).ue_initial_id,
                                         S1AP_DOWNLINK_NAS (msg_p).eNB_ue_s1ap_id);

  LOG_D(RRC, "Received %s: instance %d, ue_initial_id %d, eNB_ue_s1ap_id %d, ue_index %d\n", msg_name, instance,
        S1AP_DOWNLINK_NAS (msg_p).ue_initial_id, S1AP_DOWNLINK_NAS (msg_p).eNB_ue_s1ap_id, ue_index);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
    MessageDef *msg_fail_p;

    LOG_W(RRC, "In S1AP_DOWNLINK_NAS: unknown UE from S1AP ids (%d, %d) for eNB %d\n", S1AP_DOWNLINK_NAS (msg_p).ue_initial_id,
          S1AP_DOWNLINK_NAS (msg_p).eNB_ue_s1ap_id, instance);

    msg_fail_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_INITIAL_CONTEXT_SETUP_FAIL); // TODO change message!
    S1AP_INITIAL_CONTEXT_SETUP_FAIL (msg_fail_p).eNB_ue_s1ap_id = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id;

    itti_send_msg_to_task (TASK_S1AP, instance, msg_fail_p);

    return (-1);
  }
  else {
    /* Create message for PDCP (DLInformationTransfer_t) */
    length = do_DLInformationTransfer (&buffer, rrc_eNB_get_next_transaction_identifier (instance),
                                       S1AP_DOWNLINK_NAS (msg_p).nas_pdu.length,
                                       S1AP_DOWNLINK_NAS (msg_p).nas_pdu.buffer);

    /* Transfer data to PDCP */
    pdcp_rrc_data_req (instance, 0 /* TODO put frame number ! */, 1, (ue_index * NB_RB_MAX) + DCCH, *rrc_eNB_mui++, 0,
                       length, buffer, 1);

    return (0);
  }
}

/*------------------------------------------------------------------------------*/
int rrc_eNB_process_S1AP_INITIAL_CONTEXT_SETUP_REQ(MessageDef *msg_p, const char *msg_name, instance_t instance) {
  uint8_t ue_index;

  ue_index = get_UE_index_from_s1ap_ids (instance, S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).ue_initial_id,
                                         S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id);

  LOG_D(RRC, "Received %s: instance %d, ue_initial_id %d, eNB_ue_s1ap_id %d, nb_of_e_rabs %d, ue_index %d\n",
        msg_name, instance, S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).ue_initial_id,
        S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id, S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).nb_of_e_rabs, ue_index);

  if (ue_index == UE_INDEX_INVALID) {
    /* Can not associate this message to an UE index, send a failure to S1AP and discard it! */
    MessageDef *msg_fail_p;

    LOG_W(RRC, "In S1AP_INITIAL_CONTEXT_SETUP_REQ: unknown UE from S1AP ids (%d, %d) for eNB %d\n",
          S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).ue_initial_id, S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id, instance);

    msg_fail_p = itti_alloc_new_message (TASK_RRC_ENB, S1AP_INITIAL_CONTEXT_SETUP_FAIL);
    S1AP_INITIAL_CONTEXT_SETUP_FAIL (msg_fail_p).eNB_ue_s1ap_id = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id;

    // TODO add failure cause when defined!

    itti_send_msg_to_task (TASK_S1AP, instance, msg_fail_p);

    return (-1);
  }
  else {
    eNB_rrc_inst[instance].Info.UE[ue_index].eNB_ue_s1ap_id = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).eNB_ue_s1ap_id;

    /* Save e RAB information for later */
    {
      int i;

      eNB_rrc_inst[instance].Info.UE[ue_index].nb_of_e_rabs = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).nb_of_e_rabs;
      eNB_rrc_inst[instance].Info.UE[ue_index].index_of_e_rabs = 0;
      for (i = 0; i < eNB_rrc_inst[instance].Info.UE[ue_index].nb_of_e_rabs; i++) {
        eNB_rrc_inst[instance].Info.UE[ue_index].e_rab[i].status = E_RAB_STATUS_NEW;
        eNB_rrc_inst[instance].Info.UE[ue_index].e_rab[i].param = S1AP_INITIAL_CONTEXT_SETUP_REQ (msg_p).e_rab_param[i];
      }
    }

    {
      uint8_t send_security_mode_command = TRUE;

      // TODO evaluate if security mode command should be skipped

      if (send_security_mode_command) {
        rrc_eNB_generate_SecurityModeCommand (instance, 0 /* TODO put frame number ! */, ue_index);
      }
      else {
        rrc_eNB_generate_UECapabilityEnquiry (instance, 0 /* TODO put frame number ! */, ue_index);
      }
    }
    return (0);
  }
}

# endif /* defined(ENABLE_ITTI) */
#endif /* defined(ENABLE_USE_MME) */