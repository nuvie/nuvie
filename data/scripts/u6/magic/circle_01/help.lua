magic_casting_effect()
play_sfx(SFX_KAL_LOR, true)
party_resurrect_dead_members()
party_heal() --FIXME remove poison when healing party.
party_move(0x133, 0x160, 0)

fade_in() --FIXME the original fades from current location. We fade from black.