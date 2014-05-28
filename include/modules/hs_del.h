/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace Event
{
	struct CoreExport DeleteVhost : Events
	{
		/** Called when a vhost is deleted
		 * @param na The nickalias of the vhost
		 */
		virtual void OnDeleteVhost(NickServ::Nick *na) anope_abstract;
	};
}
