#include <protocol/shade.h>

uint8_t shade_header_size_get(shade_header *header) {
	uint8_t size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		size = 2;
	switch ((header->PFC & SHADE_PFC_PISCS_MASK) >> SHADE_PFC_PISCS_POS) {
	case (SHADE_SIZE_0):
		break;
	case (SHADE_SIZE_1):
		size += 1;
		break;
	case (SHADE_SIZE_2):
		size += 2;
		break;
	case (SHADE_SIZE_4):
		size += 4;
		break;
	default:
		break;

	}
	switch ((header->PFC & SHADE_PFC_PDSS_MASK) >> SHADE_PFC_PDSS_POS) {
	case (SHADE_SIZE_0):
		break;
	case (SHADE_SIZE_1):
		size += 1;
		break;
	case (SHADE_SIZE_2):
		size += 2;
		break;
	case (SHADE_SIZE_4):
		size += 4;
		break;
	default:
		break;

	}
	switch ((header->PFC & SHADE_PFC_PCSS_MASK) >> SHADE_PFC_PCSS_POS) {
	case (SHADE_SIZE_0):
		break;
	case (SHADE_SIZE_1):
		size += 1;
		break;
	case (SHADE_SIZE_2):
		size += 2;
		break;
	case (SHADE_SIZE_4):
		size += 4;
		break;
	default:
		break;
	}
	return size;
}

uint32_t shade_packet_size_get(shade_header *header) {
	uint32_t hs = shade_header_size_get(header);
	uint32_t ps = shade_payload_size_get(header);
	return hs + ps;
}

uint16_t shade_ptid_get(shade_header *header) {
	if (header->PFC & SHADE_PFC_SHM)
		return header->PSID; //PTIDL shifts into PSID position, PSID absent
	return ((uint16_t) header->PTIDL + ((uint16_t) header->PTIDH << 8));
}

uint8_t shade_psid_get(shade_header *header) {
	if (header->PFC & SHADE_PFC_SHM)
		return 0;
	return header->PSID;
}

uint8_t shade_pfc_get(shade_header *header) {
	return header->PFC;
}

uint8_t shade_mode_get(shade_header *header) {
	return header->PFC;
}

uint8_t shade_pisc_value_size_get(shade_header *header) {
	uint8_t size;
	switch ((header->PFC & SHADE_PFC_PISCS_MASK) >> SHADE_PFC_PISCS_POS) {
	case (SHADE_SIZE_0):
		size = 0;
		break;
	case (SHADE_SIZE_1):
		size = 1;
		break;
	case (SHADE_SIZE_2):
		size = 2;
		break;
	case (SHADE_SIZE_4):
		size = 4;
		break;
	default:
		break;
	}
	return size;
}

uint32_t shade_pisc_get(shade_header *header) {
	uint8_t size = shade_pisc_value_size_get(header);
	if (size == 0)
		return 0;
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t *base = (uint8_t*) header + header_start_size;
	uint32_t result = 0;
	for (uint8_t i = 0; i < size; ++i) {
		result |= ((uint32_t) base[i]) << (i * 8);
	}
	return result;
}

uint8_t shade_pds_value_size_get(shade_header *header) {
	uint8_t size;
	switch ((header->PFC & SHADE_PFC_PDSS_MASK) >> SHADE_PFC_PDSS_POS) {
	case (SHADE_SIZE_0):
		size = 0;
		break;
	case (SHADE_SIZE_1):
		size = 1;
		break;
	case (SHADE_SIZE_2):
		size = 2;
		break;
	case (SHADE_SIZE_4):
		size = 4;
		break;
	default:
		break;
	}
	return size;
}

uint32_t shade_pds_get(shade_header *header) {
	uint8_t size = shade_pds_value_size_get(header);
	if (size == 0)
		return 0;
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t offset = header_start_size + shade_pisc_value_size_get(header);
	uint8_t *base = (uint8_t*) header + offset;

	uint32_t result = 0;
	for (uint8_t i = 0; i < size; ++i) {
		result |= ((uint32_t) base[i]) << (i * 8);
	}
	return result;
}

uint32_t shade_payload_size_get(shade_header *header) {
	uint8_t size = shade_pds_value_size_get(header);
	if (size == 0)
		return 0;
	return shade_pds_get(header) + 1;
}

uint8_t shade_pcs_value_size_get(shade_header *header) {
	uint8_t size;
	switch ((header->PFC & SHADE_PFC_PCSS_MASK) >> SHADE_PFC_PCSS_POS) {
	case (SHADE_SIZE_0):
		size = 0;
		break;
	case (SHADE_SIZE_1):
		size = 1;
		break;
	case (SHADE_SIZE_2):
		size = 2;
		break;
	case (SHADE_SIZE_4):
		size = 4;
		break;
	default:
		break;
	}
	return size;
}

uint32_t shade_pcs_get(shade_header *header) {
	uint8_t size = shade_pcs_value_size_get(header);
	if (size == 0)
		return 0;
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t offset = header_start_size + shade_pisc_value_size_get(header) + shade_pds_value_size_get(header);
	uint8_t *base = (uint8_t*) header + offset;

	uint32_t result = 0;
	for (uint8_t i = 0; i < size; ++i) {
		result |= ((uint32_t) base[i]) << (i * 8);
	}
	return result;
}

uint8_t* shade_payload_get(shade_header *header) {
	uint32_t payload_size = shade_payload_size_get(header);
	if (payload_size == 0) {
		return (uint8_t*) 0; // No payload exists
	}
	uint32_t hs = shade_header_size_get(header);
	return (uint8_t*) header + hs;
}

void shade_pfc_set(shade_header *header, uint8_t pfc) {
	header->PFC = pfc;
}

void shade_mode_set(shade_header *header, uint8_t mode) {
	header->PFC = mode;
}

uint8_t shade_psid_set(shade_header *header, uint8_t psid) {
	if (header->PFC & SHADE_PFC_SHM)
		return 0;
	header->PSID = psid;
	return psid;
}

uint16_t shade_ptid_set(shade_header *header, uint16_t ptid) {
	if (header->PFC & SHADE_PFC_SHM) {
		header->PSID = (uint8_t) (ptid & 0xFF); //in short header mode, PTIDL is in PSID place
		return header->PSID;
	}
	header->PTIDL = (uint8_t) ((ptid >> 0) & 0xFF);
	header->PTIDH = (uint8_t) ((ptid >> 8) & 0xFF);
	return ptid;
}

uint32_t shade_pisc_set(shade_header *header, uint32_t pisc) {
	uint8_t size = shade_pisc_value_size_get(header);
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t *field = (uint8_t*) header + header_start_size;
	uint32_t real_value = 0;
	for (int i = 0; i < size; ++i) {
		field[i] = (pisc >> (8 * i)) & 0xFF;
		((uint8_t*) (&real_value))[i] = field[i];
	}
	return real_value;
}

uint32_t shade_pds_set(shade_header *header, uint32_t pds) {
	uint8_t size = shade_pds_value_size_get(header);
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t *field = (uint8_t*) header + header_start_size + shade_pisc_value_size_get(header);
	uint32_t real_value = 0;
	for (int i = 0; i < size; ++i) {
		field[i] = (pds >> (8 * i)) & 0xFF;
		((uint8_t*) (&real_value))[i] = field[i];
	}
	return real_value;
}

uint32_t shade_payload_size_set(shade_header *header, uint32_t payload_byte_size) {
	if (payload_byte_size == 0)
		return 0;
	return shade_pds_set(header, payload_byte_size - 1);
}

uint32_t shade_pcs_set(shade_header *header, uint32_t pcs) {
	uint8_t header_start_size = 4;
	if (header->PFC & SHADE_PFC_SHM)
		header_start_size = 2;
	uint8_t pisc_size = shade_pisc_value_size_get(header);
	uint8_t pds_size = shade_pds_value_size_get(header);
	uint8_t pcs_size = shade_pcs_value_size_get(header);
	uint8_t *field = (uint8_t*) header + header_start_size + pisc_size + pds_size;
	uint32_t real_value = 0;
	for (int i = 0; i < pcs_size; ++i) {
		field[i] = (pcs >> (8 * i)) & 0xFF;
		((uint8_t*) (&real_value))[i] = field[i];
	}
	return real_value;
}

void shade_payload_shade_version_set(shade_header *header) {
	uint8_t *payload = shade_payload_get(header);
	if (!payload)
		return;

	// Write enum value byte-by-byte (little-endian)
	payload[0] = (uint8_t) (SHADE_VERSION_MAJOR & 0xFF);
	payload[1] = (uint8_t) (SHADE_VERSION_MINOR & 0xFF);

	// Update packet size to include 2 bytes of payload
	shade_payload_size_set(header, 2);
}
